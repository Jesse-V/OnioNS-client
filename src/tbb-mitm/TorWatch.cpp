
#include "TorWatch.hpp"
#include "ProcessWatch.hpp"
#include <botan/botan.h>
#include <chrono>
#include <thread>
#include <fstream>
#include <sys/wait.h>


int main(int argc, char* argv[])
{
  // create socket and connect to Tor's control port
  using boost::asio::ip::tcp;
  boost::asio::io_service io;
  boost::asio::ip::tcp::socket socket(io);
  tcp::resolver resolver(io);
  tcp::resolver::query query("127.0.0.1", "9151");
  tcp::resolver::iterator iterator = resolver.resolve(query);
  boost::asio::connect(socket, iterator);

  // write message to Tor
  boost::asio::write(socket, boost::asio::buffer("protocolinfo\r\n"));

  // read from socket until newline
  boost::asio::streambuf buffer;
  boost::asio::read_until(socket, buffer, "\r\n");
  std::string response = TorWatch::toString(buffer);

  std::cout << "protocolinfo <response>" << response << "</response>"
            << std::endl;

  // write message to Tor
  boost::asio::write(socket, boost::asio::buffer("AUTHENTICATE\r\n"));

  // read from socket until newline
  boost::asio::streambuf buffer2;
  boost::asio::read_until(socket, buffer2, "\r\n");
  std::string response2 = TorWatch::toString(buffer2);

  std::cout << "auth <response>: " << response2 << "</response>" << std::endl;
}



std::string TorWatch::toString(boost::asio::streambuf& buffer)
{
  std::istream is(&buffer);
  return std::string((std::istreambuf_iterator<char>(is)),
                     std::istreambuf_iterator<char>());
}
