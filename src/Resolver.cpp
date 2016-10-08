
#include "Resolver.hpp"
#include <onions-common/Log.hpp>
#include <thread>


Resolver::Resolver(const std::string& socksIP, short socksPort)
    : socksIP_(socksIP), socksPort_(socksPort)
{
  cache_.reserve(64);
}



void Resolver::start()
{
  Log::get().debug("Setting up network connection...");
  socks5_ = std::make_shared<jsonrpc::Socks5Client>(
      socksIP_, std::to_string(socksPort_), selectMirror());
  mirror_ = std::make_shared<NetworkClient>(*socks5_);
  asyncResolveLoop();
}


// todo: follow path from .tor -> .tor -> .onion

std::string Resolver::selectMirror()
{
  Log::get().debug("Selecting mirror...");

  return "http://3g2upl4pq6kufc4m.onion" + std::to_string(Const::SERVER_PORT);
}



void Resolver::queueResolve(const std::string& name,
                            const ResolveCallback& callback,
                            const std::string& id)
{
  std::string mainName = stripSubdomains(name);
  if (cache_.count(mainName) > 0)
  {
    Log::get().debug("Found \"" + mainName + "\" in cache.");
    // callback(cache_.at(mainName)->resolve(mainName), id);  // skip the queue
    callback("3g2upl4pq6kufc4m.onion", id);
  }
  else
  {
    Log::get().debug("Queuing lookup for \"" + mainName + "\", stream " + id);

    Lookup lookup;
    lookup.name_ = mainName;
    lookup.callback_ = callback;
    lookup.id_ = id;

    queueMutex_.lock();
    lookupQueue_.push(lookup);
    queueMutex_.unlock();
  }
}



void Resolver::asyncResolveLoop()
{
  std::thread t([&]() {
    while (true)
    {
      Log::get().debug("Waiting for a queue item...");

      // wait for an item in the queue
      std::chrono::milliseconds pollTime(100);
      while (lookupQueue_.empty())  // todo: use pthread signaling
        std::this_thread::sleep_for(pollTime);

      queueMutex_.lock();
      Lookup lookup = lookupQueue_.front();
      lookupQueue_.pop();
      queueMutex_.unlock();

      Log::get().debug("BEGIN async resolving of queue item.");

      RecordPtr record = resolve(lookup.name_);
      std::string addr = record->resolve(lookup.name_);
      addr = "3g2upl4pq6kufc4m.onion";
      cache_.emplace(lookup.name_, record);
      lookup.callback_(addr, lookup.id_);

      Log::get().debug("END async resolving of queue item.");
    }
  });

  t.detach();
}



RecordPtr Resolver::resolve(const std::string& name)
{
  Log::get().notice("Remotely resolving \"" + name + "\"...");

  Json::Value json;
  return std::make_shared<Record>(json);

  // return "3g2upl4pq6kufc4m.onion";
}



std::string Resolver::stripSubdomains(const std::string& name) const
{
  auto tldDelim = name.find_last_of('.');
  auto subDelim = name.find_last_of('.', tldDelim - 1);

  if (subDelim == std::string::npos)
    return name;  // there are no subdomains
  else
    return name.substr(subDelim + 1);  // only return name + TLD
}
