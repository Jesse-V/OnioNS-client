
#include "LocalServer.hpp"
#include <onions-common/Log.hpp>


LocalServer::LocalServer(jsonrpc::AbstractServerConnector& connector)
    : LocalServerInterface(connector)
{
  Resolver::get().connectToNetwork();
}



std::string LocalServer::resolveDomain(const std::string& name)
{
  return Resolver::get().resolveDomain(name);
}



std::string LocalServer::resolveOnion(const std::string& name)
{
  return Resolver::get().resolveOnion(name);
}



bool LocalServer::isReady()
{
  return true;
  // return Resolver::get().isOnline();
}
