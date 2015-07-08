
#include <iostream>
//#include <vector>

#include <sys/wait.h>
#include <unistd.h>


char** getStemProcess()
{
  const char** args = new const char* [3];
  args[0] = "python\0";
  args[1] = "/var/lib/tor-onions/client.py\0";
  args[2] = NULL;
  return const_cast<char**>(args);
}



char** getOnionsClientProcess()
{
  const char** args = new const char* [2];
  args[0] = "onions-client\0";
  args[2] = NULL;
  return const_cast<char**>(args);
}



void startProcess(char** args)
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
      std::cout << "Process created with pid " << pid << "\n";
      wait(&pid);
      std::cout << "Process \"" << args[0] << "\" exited." << std::endl;
  }
}

int main(int argc, char* argv[])
{
  // start Tor
  std::cout << "Launching Tor..." << std::endl;
  argv[0] = const_cast<char*>("./torbin");
  startProcess(argv);

  std::cout << "Launching OnioNS client software..." << std::endl;
  startProcess(getOnionsClientProcess());  // launch onions-client

  std::cout << "Launching OnioNS-TBB software..." << std::endl;
  startProcess(getStemProcess());  // launch Stem script
}
