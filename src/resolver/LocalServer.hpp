
#include <onions-common/api/LocalServerInterface.h>

class LocalServer : public LocalServerInterface
{
 public:
  LocalServer(jsonrpc::AbstractServerConnector& connector);

  void listenForDomains(short);

  virtual bool isReady();
  virtual std::string resolveDomain(const std::string& name);
  virtual std::string resolveOnion(const std::string& name);
};
