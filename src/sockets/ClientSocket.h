
#ifndef CLIENT_SOCKET_HPP
#define CLIENT_SOCKET_HPP

#include "Socket.h"


class ClientSocket : private Socket
{
 public:
  ClientSocket(const std::string&, int);
  virtual ~ClientSocket() {}
  const ClientSocket& operator<<(const std::string&) const;
  const ClientSocket& operator>>(std::string&) const;
};

#endif
