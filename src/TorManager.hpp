
#include "sockets/ClientSocket.h"
#include <string>
#include <vector>
#include <unistd.h>

class TorManager
{
 public:
  static TorManager& get()
  {
    static TorManager instance;
    return instance;
  }

  void forkTor(int, char**);

 private:
  // singleton methods
  TorManager() {}
  TorManager(const TorManager&) = delete;
  void operator=(const TorManager&) = delete;

  // tor management
  pid_t startTor(char**);
  void manageTor();
  void waitForBootstrap(ClientSocket&);
  void authenticateToTor(ClientSocket&);
  void interceptLookups(ClientSocket&);
  void handleFreshCircuit(const std::string&, const std::string&, ClientSocket&);
  std::string getCookiePath(ClientSocket&);
  std::string getCookieHash(const std::string&);

  // low-level utilities
  std::vector<std::string> split(const char*, char c = ' ');
  pid_t startProcess(char**);
  bool isOpen(int);
};
