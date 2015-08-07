
#include "TorWatch.hpp"
#include "ProcessWatch.hpp"
#include <botan/botan.h>
#include <chrono>
#include <thread>
#include <fstream>
#include <sys/wait.h>


int main(int argc, char* argv[])
{
  TorWatch::waitForTorSOCKS();
}



void TorWatch::waitForTorSOCKS()
{
  using boost::asio::ip::tcp;
  boost::asio::io_service io;
  boost::asio::ip::tcp::socket socket(io);
  tcp::resolver resolver(io);
  tcp::resolver::query query("127.0.0.1", "9151");
  tcp::resolver::iterator iterator = resolver.resolve(query);
  boost::asio::connect(socket, iterator);

  authenticateToTor(socket);
  waitUntilBootstrapped(socket);
}



void TorWatch::authenticateToTor(boost::asio::ip::tcp::socket& socket)
{
  std::string hash = getCookieHash(getCookiePath(socket));
  const char* msg = std::string("AUTHENTICATE " + hash + "\r\n\0").c_str();

  boost::asio::write(socket, boost::asio::buffer(std::string(msg)));

  // read from socket until newline
  boost::asio::streambuf buffer;
  boost::asio::read_until(socket, buffer, "\n");
  std::string response = toString(buffer);

  if (response != "250 OK\r\n")
  {
    std::cerr << "Unexpected answer from Tor!" << std::endl;
    exit(1);
  }
}



void TorWatch::waitUntilBootstrapped(boost::asio::ip::tcp::socket& socket)
{
  std::string response;
  const std::string readyState = "BOOTSTRAP PROGRESS=100";

  while (response.find(readyState) == std::string::npos)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(250));

    // this c-string to std::string is necessary, otherwise the \0 confuses Tor
    const char* msg = "GETINFO status/bootstrap-phase\r\n\0";
    boost::asio::write(socket, boost::asio::buffer(std::string(msg)));

    // read from socket until newline
    boost::asio::streambuf buffer;
    boost::asio::read_until(socket, buffer, "\n");
    response = toString(buffer);

    std::cout << response << std::endl;
  }
}



std::string TorWatch::getCookieHash(const std::string& path)
{  // https://stackoverflow.com/questions/2602013/
  std::fstream authFile(path);

  if (!authFile)
  {
    std::cerr << "Unable to open cookie file!\n";
    exit(1);
  }

  std::string authBin((std::istreambuf_iterator<char>(authFile)),
                      std::istreambuf_iterator<char>());

  Botan::Pipe encode(new Botan::Hex_Encoder);
  encode.process_msg(authBin);
  return encode.read_all_as_string(0);
}



std::string TorWatch::getCookiePath(boost::asio::ip::tcp::socket& socket)
{
  const char* msg = "protocolinfo\r\n\0";
  boost::asio::write(socket, boost::asio::buffer(std::string(msg)));

  // read from socket until newline
  boost::asio::streambuf buffer;
  boost::asio::read_until(socket, buffer, "\n");
  std::string response = toString(buffer);

  std::cout << response << std::endl;  // todo

  std::string needle = "COOKIEFILE=";
  std::size_t pos = response.find(needle);
  if (pos == std::string::npos)
  {
    std::cerr << "Unexpected response from Tor!\n";
    exit(1);
  }

  std::size_t pathBegin = pos + needle.size() + 1;
  std::size_t pathEnd = response.find("\"", pathBegin);
  return response.substr(pathBegin, pathEnd - pathBegin);
}



std::string TorWatch::toString(boost::asio::streambuf& buffer)
{
  std::istream is(&buffer);
  return std::string((std::istreambuf_iterator<char>(is)),
                     std::istreambuf_iterator<char>());
}
