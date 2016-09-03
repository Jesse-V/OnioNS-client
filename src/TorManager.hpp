
#include <string>
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
  void waitForBootstrap();
  void authenticateToTor();
  void waitUntilBootstrapped();
  std::string getCookieHash(const std::string&);
  std::string getCookiePath();

  // C-level utilities
  pid_t startProcess(char**);
  bool isOpen(int);
};
