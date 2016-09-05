
#ifndef TOR_MANAGER_HPP
#define TOR_MANAGER_HPP

#include <onions-common/tor_ipc/ClientSocket.hpp>
#include <string>
#include <vector>
#include <memory>

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
  void requestStreamEvents(std::shared_ptr<ClientSocket>);
  void interceptLookups(std::shared_ptr<ClientSocket>);
  void handleFreshCircuit(const std::string&,
                          const std::string&,
                          std::shared_ptr<ClientSocket>);

  // low-level utilities
  pid_t startProcess(char**);
  bool isOpen(int);
};

#endif
