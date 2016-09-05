
#ifndef RESOLVER_HPP
#define RESOLVER_HPP

#include <onions-common/api/NetworkClient.h>
#include <jsonrpccpp/client/connectors/socks5client.h>
#include <memory>

// resolves using local/stored cache until connected to the network
class Resolver
{
 public:
  static Resolver& get()
  {
    static Resolver instance;
    return instance;
  }

  bool connectToNetwork();
  std::string resolveDomain(const std::string& name);
  std::string resolveOnion(const std::string& name);
  bool isOnline() const;

 private:
  // singleton methods
  Resolver();
  Resolver(const Resolver&) = delete;
  void operator=(const Resolver&) = delete;

  std::shared_ptr<NetworkClient> mirror1_, mirror2_;
  // std::shared_ptr<jsonrpc::Socks5Client> mirror1_, mirror2_;
  bool isOnline_;
};

#endif
