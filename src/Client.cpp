
#include "Client.hpp"
#include "tcp/IPC.hpp"
#include <onions-common/Common.hpp>
#include <onions-common/Config.hpp>
#include <onions-common/Constants.hpp>
#include <onions-common/Utils.hpp>
#include <iostream>


Client::Client()
{
  auto addr = Config::getMirror()[0];
  socks_ =
      SocksClient::getCircuitTo(addr["ip"].asString(), addr["port"].asInt());
  if (!socks_)
    throw std::runtime_error("Unable to connect!");
}


void Client::listenForDomains()
{
  IPC ipc(Const::IPC_PORT);
  ipc.start();
}



std::string Client::resolve(const std::string& torDomain)
{
  try
  {
    std::string domain = torDomain;

    while (Utils::strEndsWith(domain, ".tor"))
    {
      // check cache first
      auto iterator = cache_.find(domain);
      if (iterator == cache_.end())
      {
        std::cout << "Sending \"" << domain << "\" to name server...\n";
        auto received = socks_->sendReceive("domainQuery", domain);
        if (received.isMember("error"))
        {
          std::cerr << "Err: " << received["error"].asString() << std::endl;
          return "<Response_Error>";
        }
        else
          std::cout << "Received Record response." << std::endl;

        auto dest = Common::getDestination(
            Common::parseRecord(received["response"].asString()), domain);

        cache_[domain] = dest;
        domain = dest;
      }
      else
        domain = iterator->second;  // retrieve from cache
    }

    if (domain.length() != 22 || !Utils::strEndsWith(domain, ".onion"))
    {
      std::cerr << "Err: \"" + domain + "\" is not a HS address!" << std::endl;
      return "<Invalid_Result>";
    }

    return domain;
  }
  catch (std::runtime_error& re)
  {
    std::cerr << "Err: " << re.what() << std::endl;
  }

  return "<General_Error>";
}
