
#ifndef RESOLVER_HPP
#define RESOLVER_HPP

#include <onions-common/records/Record.hpp>
#include <onions-common/Constants.hpp>
#include <onions-common/api/NetworkClient.h>
#include <onions-common/jsonrpccpp/client/connectors/socks5client.h>
#include <memory>
#include <queue>
#include <unordered_map>
#include <mutex>

class Resolver
{
  typedef std::function<void(const std::string&, const std::string&)>
      ResolveCallback;

 public:
  Resolver(const std::string&, short);
  void start();
  void queueResolve(const std::string&,
                    const ResolveCallback&,
                    const std::string& id = "");

 private:
  std::string selectMirror();
  void asyncResolveLoop();
  RecordPtr resolve(const std::string&);
  std::string stripSubdomains(const std::string&) const;

  struct Lookup
  {
    std::string name_;
    std::string id_;
    ResolveCallback callback_;
  };

  std::shared_ptr<NetworkClient> mirror_;
  std::shared_ptr<jsonrpc::Socks5Client> socks5_;
  std::unordered_map<std::string, RecordPtr> cache_;
  std::queue<Lookup> lookupQueue_;
  std::mutex queueMutex_;
  const std::string socksIP_;
  short socksPort_;
};

#endif
