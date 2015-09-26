
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <onions-common/tcp/AuthenticatedStream.hpp>
#include <onions-common/containers/records/Record.hpp>
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
  bool resolveOnce(std::string&);
  RecordPtr fetchRecord(const std::string&);
  Json::Value fetchMerkleSubtree(const std::string&);
  bool fetchQuorumRootSignature(const SHA384_HASH&);

 private:
  Client() {}
  Client(Client const&) = delete;
  void operator=(Client const&) = delete;
  bool connectToResolver();

  std::shared_ptr<AuthenticatedStream> mirror_;
};

#endif
