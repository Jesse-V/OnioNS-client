
#include "IPCSession.hpp"
#include "../Client.hpp"
#include <onions-common/Log.hpp>
#include <onions-common/tcp/MemAllocator.hpp>
#include <onions-common/Utils.hpp>
#include <boost/bind.hpp>
#include <algorithm>
#include <fstream>


template <typename Handler>
inline MemAllocator<Handler> makeHandler(HandleAlloc& a, Handler h)
{
  return MemAllocator<Handler>(a, h);
}



IPCSession::IPCSession(boost::asio::io_service& ios) : socket_(ios)
{
}



boost::asio::ip::tcp::socket& IPCSession::getSocket()
{
  return socket_;
}



void IPCSession::start()
{
  asyncReadBuffer();
}



void IPCSession::processRead(const boost::system::error_code& error, size_t n)
{
  if (n == 0)
  {
    Log::get().notice("IPC connection terminated.");
    return;
  }
  else if (error)
  {
    Log::get().warn("IPC: " + error.message());
    return;
  }

  // Client is a singleton because of this code. Todo: reorganize
  std::string domainIn(buffer_.begin(), buffer_.begin() + n);
  domainIn = Utils::trimString(domainIn);
  Log::get().notice("Read \"" + domainIn + "\" from IPC.");
  std::string onionOut = Client::get().resolve(domainIn);
  Log::get().notice("Writing \"" + onionOut + "\" to IPC... ");

  for (std::size_t j = 0; j < onionOut.size(); j++)
    buffer_[j] = onionOut[j];
  asyncWriteBuffer(onionOut.size());
}



// called by Asio when the buffer has been written to the socket
void IPCSession::processWrite(const boost::system::error_code& error)
{
  if (error)
  {
    Log::get().warn("IPC processWrite: " + error.message());
    return;
  }

  Log::get().notice("Write complete.");
  asyncReadBuffer();
}



// ***************************** PRIVATE METHODS *****************************



void IPCSession::asyncReadBuffer()
{
  socket_.async_read_some(
      boost::asio::buffer(buffer_),
      makeHandler(allocator_,
                  boost::bind(&IPCSession::processRead, shared_from_this(),
                              boost::asio::placeholders::error,
                              boost::asio::placeholders::bytes_transferred)));
}



void IPCSession::asyncWriteBuffer(std::size_t len)
{
  boost::asio::async_write(
      socket_, boost::asio::buffer(buffer_, len),
      makeHandler(allocator_,
                  boost::bind(&IPCSession::processWrite, shared_from_this(),
                              boost::asio::placeholders::error)));
}
