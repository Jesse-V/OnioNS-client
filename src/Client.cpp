
#include "Client.hpp"
#include "tcp/IPC.hpp"
#include <onions-common/Common.hpp>
#include <onions-common/containers/MerkleTree.hpp>
#include <onions-common/containers/Cache.hpp>
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

  try
  {
    Log::get().notice("Connecting to name server...");

    mirror_ = std::make_shared<AuthenticatedStream>(
        "127.0.0.1", socksPort, M_ONION, SERVER_PORT, M_KEY);
  }
  catch (const std::runtime_error& re)
  {
    Log::get().error("SOCKS responded: " + std::string(re.what()));
  }

  IPC ipc(Const::IPC_PORT);
  ipc.start();
}



std::string Client::resolve(const std::string& torDomain)
{
  try
  {
    std::string domain = torDomain;

    while (Utils::strEndsWith(domain, ".tor"))
      if (!resolveOnce(domain))
        return "<error_resolving>";

    if (domain.length() != 22 || !Utils::strEndsWith(domain, ".onion"))
    {
      Log::get().warn("\"" + domain + "\" is not a HS address!");
      return "<invalid_request>";
    }

    return domain;
  }
  catch (std::runtime_error& re)
  {
    Log::get().error(re.what());
    return "<general_error>";
  }
}



bool Client::resolveOnce(std::string& domain)
{
  // check cache first
  RecordPtr record = Cache::get(domain);
  if (!record)
  {  // try to remotely resolve

    record = fetchRecord(domain);
    if (!record)
      return false;

    Cache::add(record);

    // todo: we can cache the subtrees to avoid refetching every time
    Json::Value subtreeObj = fetchMerkleSubtree(domain);
    if (!MerkleTree::doesContain(subtreeObj, record))
      return false;

    if (!fetchQuorumRootSignature(MerkleTree::extractRoot(subtreeObj)))
      return false;
  }

  // Log::get().notice("Record was successfully authenticated.");
  domain = Common::getDestination(record, domain);
  return true;
}



RecordPtr Client::fetchRecord(const std::string& domain)
{
  Log::get().notice("Remotely resolving \"" + domain + "\" to a Record...");
  Json::Value response = mirror_->sendReceive("domainQuery", domain);
  Log::get().notice("Received Record from name server.");

  if (response["type"] == "error")
  {
    Log::get().warn("Received " + response["value"].asString());
    return nullptr;
  }

  return Common::parseRecord(response["value"].asString());
}



Json::Value Client::fetchMerkleSubtree(const std::string& domain)
{
  Log::get().notice("Fetching Merkle subtree for Record...");
  Json::Value response = mirror_->sendReceive("getMerkleSubtree", domain);
  Log::get().notice("Received Merkle subtree from name server.");

  return response["value"];
}



bool Client::fetchQuorumRootSignature(const SHA384_HASH& root)
{
  auto response = mirror_->sendReceive("getRootSignature", "");

  if (response["type"] == "error")
  {
    Log::get().warn("Error when getting root signature from Quorum node: " +
                    response["value"].asString());
    return false;
  }
  else
  {
    static auto Q_KEY = Config::getQuorumNode()[0]["key"].asString();
    ED_SIGNATURE sig;
    auto result =
        Common::verifyRootSignature(response["value"], sig, root, Q_KEY);

    // MerkleTree::extractRoot

    // todo: MerkleTree::verifyRoot
    return result.first;
  }
}
