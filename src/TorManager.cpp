
#include "TorManager.hpp"
#include <onions-common/Log.hpp>
#include <onions-common/Utils.hpp>
#include <thread>
#include <cstring>
#include <csignal>
#include <unistd.h>


TorManager::TorManager(const std::string& ip,
                       short port,
                       const std::shared_ptr<Resolver>& resolver)
    : controller_(std::make_shared<TorController>(ip, port)),
      resolver_(resolver)
{
}



void TorManager::start(int argc, char* argv[])
{
  // there are more reliable ways, but this works decently
  bool startedTor = true;
  pid_t torP = -1;  // startTor(argv);
  if (argc < 13 || torP <= 0)
  {
    Log::get().warn("Failed to start torbin.");
    startedTor = false;
  }

  waitForControlPort(startedTor);
  resolver_->start();
  manageTor();

  kill(torP, SIGTERM);
}



void TorManager::waitForControlPort(bool startedTor)
{
  if (!startedTor && !controller_->connect())
    Log::get().error("Could not connect to the Tor's control port.");

  std::chrono::milliseconds pollTime(333);
  while (!controller_->connect())
  {
    Log::get().warn("Failed to connect to Tor's control port, retrying...");
    std::this_thread::sleep_for(pollTime);
  }

  Log::get().warn("Connected to control port.");
}



void TorManager::manageTor()
{
  Log::get().notice("Attempting to authenticate...");
  controller_->authenticateToTor();
  Log::get().notice("Waiting for Tor to finish bootstrapping...");
  controller_->waitForBootstrap();
  Log::get().notice("Tor bootstrap complete.");

  if (!controller_->setSetting("__LeaveStreamsUnattached", "1"))
    Log::get().warn("Couldn't leave streams unattached!");

  if (controller_->command("SETEVENTS stream"))
    interceptLookups();
}



void TorManager::interceptLookups()
{
  Log::get().notice("Intercepting stream requests...");

  while (true)
  {
    std::string line = controller_->getClientSocket()->readLine();
    if (line == "250 OK")
    {
      Log::get().debug("Stream command acknowledged.");
      continue;  // it comes from the callback
    }

    auto words = Utils::split(line.c_str());
    if (words[5].find(".exit:") != std::string::npos)
      continue;  // if it's an internal stream

    // 650 STREAM 360 NEW 0 example.tor:80 SOURCE_ADDR=127.0.0.1:12345
    // PURPOSE=USER

    // see
    // https://gitweb.torproject.org/torspec.git/tree/control-spec.txt#n1853
    if (words[1] == "STREAM" && words[3] == "NEW" && words[4] == "0")
    {
      Log::get().debug("New stream event detected.");
      handleFreshCircuit(words[2], words[5].substr(0, words[5].find(':')));
    }
  }
}



void TorManager::handleFreshCircuit(const std::string& id,
                                    const std::string& domain)
{
  if (domain.find(".tor") == std::string::npos)
  {  // don't care about this stream, so auto-attach
    Log::get().debug("Auto-attaching new stream: \"" + domain + "\" - " + id);
    controller_->command("ATTACHSTREAM " + id + " 0");
  }
  else
  {
    Log::get().notice("Resolving \"" + domain + "\" for stream " + id);
    resolver_->queueResolve(
        domain,
        [&](const std::string& addr, const std::string& id) {
          Log::get().debug("Entered callback with \"" + addr + "\"");
          auto socket = controller_->getClientSocket();

          if (addr.empty())
          {
            Log::get().notice("Resolve failed. Closing stream " + id);
            socket->writeLine("CLOSESTREAM " + id + " 2");
          }
          else
          {
            // Log::get().notice("Redirecting \"" + domain + "\" to \"" +
            // address + "\"");
            Log::get().notice("Redirecting " + id + " to \"" + addr + "\"");
            socket->writeLine("REDIRECTSTREAM " + id + " " + addr);
            socket->writeLine("ATTACHSTREAM " + id + " 0");
            // responses will be picked up in interceptLookups()
          }

          Log::get().debug("Callback complete.");
        },
        id);
  }
}



pid_t TorManager::startTor(char** argv)
{
  argv[0] = const_cast<char*>("TorBrowser/Tor/torbin");
  return startProcess(argv);
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
      Log::get().debug("Child process has pid " + std::to_string(pid));
      // wait(&status); /* wait for child to exit, and store child's exit status
      //
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
