
#ifndef SOCKET_EXCEPTION_HPP
#define SOCKET_EXCEPTION_HPP

#include <string>

class SocketException
{
 public:
  SocketException(const std::string& s) : message_(s) {}
  ~SocketException() {}
  std::string description() { return message_; }
 private:
  std::string message_;
};

#endif
