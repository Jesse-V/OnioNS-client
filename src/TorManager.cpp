
#include "TorManager.hpp"
#include "resolver/Resolver.hpp"
#include <onions-common/tor_ipc/TorController.hpp>
#include <onions-common/tor_ipc/SocketException.hpp>
#include <onions-common/Log.hpp>
#include <onions-common/Utils.hpp>
#include <thread>
#include <string.h>


void TorManager::forkTor(int argc, char* argv[])
{
  /*
  temporarily commented out
  if (argc < 13)  // there are more reliable ways, but this works decently
  {
    Log::get().notice("Tor Browser not detected; won't manage Tor.");
    return;
  }

  //pid_t torP = startTor(argv);
  if (startTor(argv) <= 0)
  {
    Log::get().warn("Failed to start Tor, will run normal local listener.");
    return;
  }
  */

  manageTor();

  /*
    std::chrono::milliseconds pollTime(250);
    while (ProcessWatch::isRunning(torP))
      std::this_thread::sleep_for(pollTime);
  */

  // I learned in college that you have to kill your children before killing
  // yourself
  // kill(torP, SIGTERM);
}



pid_t TorManager::startTor(char** argv)
{
  argv[0] = const_cast<char*>("TorBrowser/Tor/torbin");
  pid_t torP = startProcess(argv);
  if (torP <= 0)
    return torP;

  // wait for Tor's control port to be available
  while (!isOpen(9151))
  {
    // https://stackoverflow.com/questions/5278582
    // if (waitpid(torP, NULL, WNOHANG) != 0)  // test for existence of Tor
    //  exit(0);  // if the Tor Browser was closed, then quit
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
  }

  Log::get().notice("Tor's control port is now available.");

  return torP;
}



void TorManager::manageTor()
{
  try
  {
    TorController control("127.0.0.1", 9151);

    Log::get().notice("Attempting to authenticate to Tor...");
    control.authenticateToTor();
    Log::get().notice("Waiting for Tor to finish bootstrapping...");
    control.waitForBootstrap();
    Log::get().notice("Tor bootstrap complete.");
    control.setSetting("__LeaveStreamsUnattached", "1");
    requestStreamEvents(control.getSocket());
    interceptLookups(control.getSocket());
  }
  catch (SocketException& e)
  {
    Log::get().warn("Communication error with Tor's control port! " +
                    e.description());
  }
}



void TorManager::requestStreamEvents(std::shared_ptr<ClientSocket> socket)
{
  std::string response;
  *socket << "SETEVENTS stream\r\n";
  *socket >> response;
  if (response != "250 OK\r\n")
    Log::get().warn("Couldn't request event notifications.");
}



void TorManager::interceptLookups(std::shared_ptr<ClientSocket> socket)
{
  std::string response;
  while (true)
  {
    *socket >> response;
    auto lines = Utils::split(response.c_str(), '\r');
    for (auto line : lines)
    {
      auto words = Utils::split(line.c_str());
      // 650 STREAM 360 NEW 0 example.tor:80 SOURCE_ADDR=127.0.0.1:12345
      // PURPOSE=USER

      // if it's a fragmented response string or if it's an internal stream
      if (words[0].size() != 3 || words[5].find(".exit:") != std::string::npos)
        continue;

      if (words[0] != "650" && words[0] != "250")
        Log::get().warn("Tor issue: \"" + line + "\"");

      // see
      // https://gitweb.torproject.org/torspec.git/tree/control-spec.txt#n1853
      if (words[1] == "STREAM" && words[3] == "NEW" && words[4] == "0")
        handleFreshCircuit(words[2], words[5], socket);
    }
  }
}



void TorManager::handleFreshCircuit(const std::string& streamID,
                                    const std::string& domain,
                                    std::shared_ptr<ClientSocket> socket)
{
  if (domain.find(".tor:") == std::string::npos)
  {  // don't care about this stream, so auto-attach

    Log::get().notice("Auto-attaching new stream: \"" + domain + "\"");
    *socket << "ATTACHSTREAM " << streamID << " 0\r\n";
  }
  else
  {
    Log::get().notice("Found request to \"" + domain + "\"");

    std::string address = Resolver::get().resolveOnion(domain);
    if (address.empty())
    {
      Log::get().notice("Failed to resolve \"" + domain +
                        "\". Closing stream.");
      *socket << "CLOSESTREAM " << streamID << " 2\r\n";
    }
    else
    {
      Log::get().notice("Rewriting \"" + domain + "\" to \"" + address + "\"");
      *socket << "REDIRECTSTREAM " << streamID << " " << address
              << "\r\n";  // todo: handle 443
      *socket << "ATTACHSTREAM " << streamID << " 0\r\n";
    }
  }
}



// start a child process based on the given commands
pid_t TorManager::startProcess(char** args)
{  // http://www.cplusplus.com/forum/lounge/17684/
   // http://www.thegeekstuff.com/2012/03/c-process-control-functions/

  Log::get().notice("Starting " + std::string(args[0]) + "...");
  pid_t pid = fork();

  if (pid >= 0)
  {
    if (pid == 0)
    {
      execve(args[0], args, NULL);
      Log::get().warn("Failed to execute process. " +
                      std::string(strerror(errno)));
      return -1;
      // Pid after: " + std::to_string(pid) + "," + std::to_string(ret));
      // exit(1);
    }
    else
    {
      // int status;
      Log::get().notice("Child process has pid " + std::to_string(pid));
      // wait(&status); /* wait for child to exit, and store child's exit status
      // */
      // Log::get().notice("Child exit code: " +
      // std::to_string(WEXITSTATUS(status)));
    }
  }
  else
  {
    Log::get().error("fork() failed!");
  }

  return pid;
}



// test if a localhost port is open or not
bool TorManager::isOpen(int port)
{  // https://theredblacktree.wordpress.com/2013/09/30/how-to-check-if-a-port-is-open-or-not-in-c-unixlinux/

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    Log::get().warn("Error opening socket!");
    return false;
  }

  struct hostent* server = gethostbyname("127.0.0.1");
  if (server == NULL)
  {
    Log::get().warn("ERROR, no such host");
    return false;
  }

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  memcpy(&server->h_addr, &serv_addr.sin_addr.s_addr,
         static_cast<size_t>(server->h_length));

  serv_addr.sin_port = htons(port);
  bool isOpen = connect(sockfd, reinterpret_cast<struct sockaddr*>(&serv_addr),
                        sizeof(serv_addr)) >= 0;
  close(sockfd);
  return isOpen;
}


/*
bool TorManager::isRunning(pid_t p)
{  // http://www.unix.com/programming/121343-c-wexitstatus-question.html

  errno = 0;
  if (kill(p, 0) == 0)
    return true;
  else
  {
    if (errno == ESRCH)
      Log::get().notice("Child process " + std::to_string(p) + " has exited.");
    else
      Log::get().warn("Could not signal process " + std::to_string(p) + "!");

    return false;
  }
}
*/
