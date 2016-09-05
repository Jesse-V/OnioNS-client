
#ifndef LOCAL_SERVER_HPP
#define LOCAL_SERVER_HPP

#include "Resolver.hpp"
#include <onions-common/api/LocalServerInterface.h>

class LocalServer : public LocalServerInterface
{
 public:
  LocalServer(jsonrpc::AbstractServerConnector& connector);

  virtual std::string resolveDomain(const std::string& name);
  virtual std::string resolveOnion(const std::string& name);
  virtual bool isReady();  // todo: do I need this method?

 private:
  std::shared_ptr<Resolver> resolver_;
};

#endif
