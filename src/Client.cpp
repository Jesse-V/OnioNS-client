
#include "Client.hpp"
#include "tcp/IPC.hpp"
#include <onions-common/Common.hpp>
#include <onions-common/Log.hpp>
#include <onions-common/Config.hpp>
#include <onions-common/Constants.hpp>
#include <onions-common/Utils.hpp>


void Client::listenForDomains(short socksPort)
{
  const auto M_NODE = Config::getMirror()[0];
  const auto M_ONION = M_NODE["addr"].asString();
  const auto M_KEY = M_NODE["key"].asString();
  const auto SERVER_PORT = Const::SERVER_PORT;

  mirror_ = std::make_shared<AuthenticatedStream>("127.0.0.1", socksPort,
                                                  M_ONION, SERVER_PORT, M_KEY);

  IPC ipc(Const::IPC_PORT);
  ipc.start();
}



std::string Client::resolve(const std::string& torDomain)
{
  if (torDomain == "check.torproject.org")
    return "onions.jessevictors.com";

  try
  {
    std::string domain = torDomain;

    while (Utils::strEndsWith(domain, ".tor"))
    {
      // check cache first
      auto iterator = cache_.find(domain);
      if (iterator == cache_.end())
      {
        std::string destination = remotelyResolve(domain);
        cache_[domain] = destination;
        domain = destination;
      }
      else
        domain = iterator->second;  // retrieve from cache
    }

    if (domain.length() != 22 || !Utils::strEndsWith(domain, ".onion"))
    {
      Log::get().warn("\"" + domain + "\" is not a HS address!");
      return "<Invalid_Request>";
    }

    return domain;
  }
  catch (std::runtime_error& re)
  {
    Log::get().error(re.what());
  }

  return "<General_Error>";
}



std::string Client::remotelyResolve(const std::string& domain)
{
  // get Record from name server
  Json::Value recordObj = fetchRecord(domain);
  if (recordObj["type"] == "error")
  {
    Log::get().warn(recordObj["value"].asString());
    return "<not-found>";
  }

  RecordPtr record = Common::parseRecord(recordObj["value"].asString());
  if (!record)
  {
    Log::get().warn("Failed to parse Record!");
    return "<failed-to-parse-record>";
  }

  Json::Value subtreeObj = fetchMerkleSubtree(domain);
  if (subtreeObj["type"] == "error")
  {
    Log::get().warn(subtreeObj["value"].asString());
    return "<failed-authentication>";
  }

  /*
  if (!MerkleTree::doesContain(subtreeJSON, record))
  {
    Log::get().warn("The Record cannot be authenticated!");
    return "<Record cannot be authenticated>"; //520, general error
  }

  auto qSignatureJSON = fetchQuorumRootSignature(domain);
  if (!verifyRootSignature(subtreeJSON, qSignatureJSON))
  {
    Log::get().warn("Invalid signature on Merkle root.");
    return "<Invalid root signature>";
  }

  Log::get().notice("Record was successfully authenticated.");
*/

  Log::get().notice(subtreeObj["value"].asString());


  return Common::getDestination(record, domain);
}



Json::Value Client::fetchRecord(const std::string& domain)
{
  Log::get().notice("Remotely resolving \"" + domain + "\" to a Record...");
  Json::Value response = mirror_->sendReceive("domainQuery", domain);
  Log::get().notice("Received Record from name server.");

  return response;
}



Json::Value Client::fetchMerkleSubtree(const std::string& domain)
{
  Log::get().notice("Fetching Merkle subtree for Record...");
  Json::Value response = mirror_->sendReceive("getMerkleSubtree", domain);
  Log::get().notice("Received Merkle subtree from name server.");

  return response;
}
