
#ifndef TOR_MANAGER_HPP
#define TOR_MANAGER_HPP

#include "Resolver.hpp"
#include <onions-common/ipc/TorController.hpp>
#include <string>
#include <memory>

class TorManager
{
 public:
  TorManager(const std::string&, short, const std::shared_ptr<Resolver>&);
  void start(int, char**);

 private:
  // tor management
  void waitForControlPort(bool);
  void manageTor();
  void interceptLookups();
  void handleFreshCircuit(const std::string&, const std::string&);
  void asyncResolveLoop();

  // low-level utilities
  pid_t startTor(char**);
  pid_t startProcess(char**);
  bool isOpen(int);

  std::shared_ptr<TorController> controller_;
  std::shared_ptr<Resolver> resolver_;
};

#endif
