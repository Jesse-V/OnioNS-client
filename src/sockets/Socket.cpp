
#include "Socket.h"
#include "string.h"
#include <string.h>
#include <errno.h>
#include <fcntl.h>


Socket::Socket() : sock_(-1)
{
  memset(&addr_, 0, sizeof(addr_));
}



Socket::~Socket()
{
  if (isValid())
    ::close(sock_);
}



bool Socket::create()
{
  sock_ = socket(AF_INET, SOCK_STREAM, 0);

  if (!isValid())
    return false;

  int on = 1;
  return setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, (const char*)&on,
                 sizeof(on)) != -1;
}



bool Socket::bind(int port)
{
  if (!isValid())
    return false;

  addr_.sin_family = AF_INET;
  addr_.sin_addr.s_addr = INADDR_ANY;
  addr_.sin_port = htons(port);

  return ::bind(sock_, reinterpret_cast<struct sockaddr*>(&addr_),
                sizeof(addr_)) != -1;
}



bool Socket::listen() const
{
  if (!isValid())
    return false;

  return ::listen(sock_, MAXCONNECTIONS) != -1;
}



bool Socket::accept(Socket& newSocket) const
{
  int addrLen = sizeof(addr_);
  newSocket.sock_ = ::accept(sock_, (sockaddr*)&addr_,
                             reinterpret_cast<socklen_t*>(&addrLen));
  return newSocket.sock_ > 0;
}



bool Socket::send(const std::string& s) const
{
  ssize_t status = ::send(sock_, s.c_str(), s.size(), MSG_NOSIGNAL);
  return status != -1;
}



int Socket::recv(std::string& s) const
{
  char buf[MAXRECV + 1];
  s = "";
  memset(buf, 0, MAXRECV + 1);

  ssize_t status = ::recv(sock_, buf, MAXRECV, 0);

  if (status == 0 || status == -1)
    return 0;
  else
  {
    s = buf;
    return static_cast<int>(status);
  }
}



bool Socket::connect(const std::string& host, int port)
{
  if (!isValid())
    return false;

  addr_.sin_family = AF_INET;
  addr_.sin_port = htons(port);

  int status = inet_pton(AF_INET, host.c_str(), &addr_.sin_addr);
  if (errno == EAFNOSUPPORT)
    return false;

  status = ::connect(sock_, (sockaddr*)&addr_, sizeof(addr_));
  return status == 0;
}



void Socket::setNonBlocking(bool b)
{
  int opts = fcntl(sock_, F_GETFL);
  if (opts < 0)
    return;

  if (b)
    opts = (opts | O_NONBLOCK);
  else
    opts = (opts & ~O_NONBLOCK);

  fcntl(sock_, F_SETFL, opts);
}



bool Socket::isValid() const
{
  return sock_ != -1;
}
