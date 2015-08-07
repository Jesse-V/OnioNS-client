
#ifndef PROCESS_WATCH_HPP
#define PROCESS_WATCH_HPP

#include <unistd.h>

class ProcessWatch
{
 public:
  static pid_t launchTor(char* argv[]);
  static pid_t launchOnioNS(pid_t torP);
  static pid_t launchStem();

  static pid_t startProcess(char** args);
  static bool isOpen(int port);
  static char** getStemProcess();
  static char** getOnionsClientProcess();
};

#endif
