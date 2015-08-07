
#ifndef TOR_WATCH_HPP
#define TOR_WATCH_HPP

#include <boost/asio.hpp>
#include <unistd.h>
#include <string>

class TorWatch
{
 public:
  static void waitForTorDeath(pid_t torP);
  static void waitForTorSOCKS();
  static void authenticateToTor(boost::asio::ip::tcp::socket&);
  static void waitUntilBootstrapped(boost::asio::ip::tcp::socket&);
  static std::string getCookieHash(const std::string&);
  static std::string getCookiePath(boost::asio::ip::tcp::socket&);
  static std::string toString(boost::asio::streambuf&);
};

#endif
