
#include "LocalServer.hpp"
#include <onions-common/Log.hpp>


LocalServer::LocalServer(jsonrpc::AbstractServerConnector& connector)
    : LocalServerInterface(connector)
{
}



bool LocalServer::isReady()
{
  return true;
}



std::string LocalServer::resolveDomain(const std::string& name)
{
  Log::get().notice("Resolving \"" + name + "\"...");
  std::string address = "address.onion";
  Log::get().notice("Mapped \"" + name + "\" to \"" + address + "\".");

  return address;
}



std::string LocalServer::resolveOnion(const std::string& address)
{
  Log::get().notice("Resolving \"" + address + "\"...");
  std::string domain = "example.tor";
  Log::get().notice("Mapped \"" + address + "\" to \"" + domain + "\".");

  return domain;
}
