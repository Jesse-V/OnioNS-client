
#ifndef PROCESS_WATCH_HPP
#define PROCESS_WATCH_HPP

#include <unistd.h>

class ProcessWatch
{
 public:
  static pid_t launchTor(char**);
  static pid_t launchOnioNS(pid_t);
  static pid_t launchStem();

  static pid_t startProcess(char**);
  static bool isOpen(int port);
  static bool isRunning(pid_t);
  static char** getStemProcess();
  static char** getOnionsClientProcess();
};

#endif
