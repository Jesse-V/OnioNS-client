
#ifndef TOR_WATCH_HPP
#define TOR_WATCH_HPP

#include <boost/asio.hpp>
#include <unistd.h>
#include <string>

class TorWatch
{
 public:
  static void waitForTorSOCKS();
  static void authenticateToTor(boost::asio::ip::tcp::socket& socket);
  static void waitUntilBootstrapped(boost::asio::ip::tcp::socket& socket);
  static std::string getCookieHash();
};

#endif
