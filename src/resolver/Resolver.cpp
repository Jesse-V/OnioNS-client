
#include "Resolver.hpp"
#include <onions-common/Log.hpp>
#include <onions-common/api/NetworkClient.h>
#include <onions-common/jsonrpccpp/client/connectors/socks5client.h>

/*
  Socks5Client mirror1("127.0.0.1", 9150, "http://address.onion");
  NetworkClient c(httpclient);

  try
  {
    cout << c.sayHello("Peter") << endl;
    c.notifyServer();
  }
  catch (JsonRpcException e)
  {
    cerr << e.what() << endl;
  }
}*/


Resolver::Resolver() : isOnline_(false)
{
}



bool Resolver::connectToNetwork()
{
  if (isOnline_)
  {
    Log::get().warn("Resolver is already online!");
    return true;
  }

  // try
  {
    // todo: finish. We need to try 16 mirrors before failing.
    // use thread with mutexes?
    /*
    mirror1_ = std::make_shared<NetworkClient>(
        Socks5Client("127.0.0.1", 9150, "http://address.onion"));
    mirror2_ = std::make_shared<NetworkClient>(
        Socks5Client("127.0.0.1", 9150, "http://address.onion"));*/
    isOnline_ = true;
  }
  /*catch (SocketException& ex)
  {
    Log::get().warn("Error connecting to mirror:  " + ex.description());
    return false;
  }*/

  Log::get().notice("Resolver has connected to the OnioNS network.");
  return true;
}



std::string Resolver::resolveDomain(const std::string& name)
{
  Log::get().notice("Resolving \"" + name + "\"...");

  /*
  if (in cache AND cache hasn't expired)
  {
    Log::get().notice("Found record in cache.");
    return record.getOnionAddress();
  }
  */

  std::string address = "3g2upl4pq6kufc4m.onion";
  // if (isOnline_)
  {
      // Log::get().notice("Resolving \"" + name + "\" remotely...");
      // todo. Get record, verify proof, cache it, etc
  }  // else
  {
    // Log::get().warn("Cache miss and not online, failing lookup.");
    // return std::string();
  }

  Log::get().notice("Mapped \"" + name + "\" to \"" + address + "\".");
  return address;
}



std::string Resolver::resolveOnion(const std::string& address)
{
  Log::get().notice("Resolving \"" + address + "\"...");
  std::string domain = "example.tor";
  Log::get().notice("Mapped \"" + address + "\" to \"" + domain + "\".");

  return domain;
}



bool Resolver::isOnline() const
{
  return isOnline_;
}
