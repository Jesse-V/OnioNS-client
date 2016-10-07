
#ifndef TOR_MANAGER_HPP
#define TOR_MANAGER_HPP

#include <onions-common/ipc/TorController.hpp>
#include <string>
#include <memory>

class TorManager
{
 public:
  TorManager(const std::string&, short);
  void forkTor(int, char**);

 private:
  // tor management
  pid_t startTor(char**);
  void manageTor();
  void interceptLookups();
  void handleFreshCircuit(const std::string&, const std::string&);

  // low-level utilities
  pid_t startProcess(char**);
  bool isOpen(int);

  std::string controlIP_;
  short controlPort_;
  std::shared_ptr<TorController> controller_;
};

#endif
