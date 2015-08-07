
#include "ProcessWatch.hpp"
#include <chrono>
#include <thread>
#include <fstream>
#include <iostream>
#include <sys/wait.h>
#include <string.h>
#include <netdb.h>


pid_t ProcessWatch::launchTor(char* argv[])
{
  std::cout << "Launching Tor..." << std::endl;
  argv[0] = const_cast<char*>("TorBrowser/Tor/torbin");
  pid_t torP = startProcess(argv);

  // wait for Tor's control port to be available
  while (!isOpen(9151))
  {
    // https://stackoverflow.com/questions/5278582
    if (waitpid(torP, NULL, WNOHANG) != 0)  // test for existence of Tor
      exit(0);  // if the Tor Browser was closed, then quit
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
  }

  return torP;
}



pid_t ProcessWatch::launchOnioNS(pid_t torP)
{
  // start the onions-client executable
  std::cout << "Launching OnioNS client software..." << std::endl;
  pid_t ocP =
      ProcessWatch::startProcess(ProcessWatch::getOnionsClientProcess());

  // wait until onions-client has established a connection
  while (!ProcessWatch::isOpen(9053))
  {
    if (waitpid(torP, NULL, WNOHANG) != 0 || waitpid(ocP, NULL, WNOHANG) != 0)
    {
      // kill children
      kill(torP, SIGQUIT);
      kill(ocP, SIGQUIT);

      return EXIT_FAILURE;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(250));
  }

  return ocP;
}



pid_t ProcessWatch::launchStem()
{
  // launch the Stem script
  std::cout << "Launching OnioNS-TBB software..." << std::endl;
  return ProcessWatch::startProcess(ProcessWatch::getStemProcess());
}



// start a child process based on the given commands
pid_t ProcessWatch::startProcess(char** args)
{  // http://www.cplusplus.com/forum/lounge/17684/
  pid_t pid = fork();

  switch (pid)
  {
    case -1:
      std::cerr << "Uh-Oh! fork() failed.\n";
      exit(1);
    case 0:                   // Child process
      execvp(args[0], args);  // Execute the program
      std::cerr << "Failed to execute process!\n";
      exit(1);
    default: /* Parent process */
      std::cout << "Process \"" << args[0] << "\" created, pid " << pid << "\n";
      // wait(&pid);
      return pid;
  }
}



// test if a localhost port is open or not
bool ProcessWatch::isOpen(int port)
{  // https://theredblacktree.wordpress.com/2013/09/30/how-to-check-if-a-port-is-open-or-not-in-c-unixlinux/

  std::cout << "Testing..." << std::endl;
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    std::cerr << "Error opening socket!" << std::endl;
    return false;
  }

  struct hostent* server = gethostbyname("127.0.0.1");
  if (server == NULL)
  {
    std::cerr << "ERROR, no such host" << std::endl;
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
  std::cout << "Result: " << isOpen << std::endl;
  return isOpen;
}



// get command for launching the Stem script
char** ProcessWatch::getStemProcess()
{
  const char** args = new const char* [3];
  args[0] = "python\0";
  args[1] = "/var/lib/tor-onions/client.py\0";
  args[2] = NULL;
  return const_cast<char**>(args);
}



// get commmand for launching the onions-client executable
char** ProcessWatch::getOnionsClientProcess()
{
  const char** args = new const char* [4];
  args[0] = "onions-client\0";
  args[1] = "-o\0";
  args[2] = "TorBrowser/OnioNS/client.log\0";
  args[3] = NULL;
  return const_cast<char**>(args);
}
