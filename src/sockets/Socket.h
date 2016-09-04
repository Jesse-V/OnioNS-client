
#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>

const int MAXCONNECTIONS = 16;
const int MAXRECV = 2048;

class Socket
{
 public:
  Socket();
  virtual ~Socket();

  bool create();
  bool bind(int);
  bool listen() const;
  bool accept(Socket&) const;
  bool connect(const std::string&, int);

  bool send(const std::string&) const;
  int recv(std::string&) const;

  void setNonBlocking(bool);
  bool isValid() const;

 private:
  int sock_;
  sockaddr_in addr_;
};

#endif
