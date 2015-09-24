
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <onions-common/tcp/AuthenticatedStream.hpp>
#include <unordered_map>
#include <memory>
#include <string>

class Client
{
 public:
  static Client& get()
  {
    static Client instance;
    return instance;
  }

  void listenForDomains(short);
  std::string resolve(const std::string&);
  std::string remotelyResolve(const std::string&);
  Json::Value fetchRecord(const std::string&);
  Json::Value fetchMerkleSubtree(const std::string&);

 private:
  Client() {}
  Client(Client const&) = delete;
  void operator=(Client const&) = delete;
  bool connectToResolver();

  std::shared_ptr<AuthenticatedStream> mirror_;
  std::unordered_map<std::string, std::string> cache_;
};

#endif
