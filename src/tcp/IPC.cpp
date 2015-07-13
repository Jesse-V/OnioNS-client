
#include "IPC.hpp"
#include <onions-common/Log.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;


IPC::IPC(ushort port)
    : ios_(std::make_shared<boost::asio::io_service>()),
      acceptor_(
          *ios_,
          tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"),
                        port))
{
  Log::get().notice("Initiating Stem-OnioNS IPC socket...");
  boost::shared_ptr<IPCSession> session(new IPCSession(*ios_));
  acceptor_.async_accept(session->getSocket(),
                         boost::bind(&IPC::handleAccept, this, session,
                                     boost::asio::placeholders::error));
}



IPC::~IPC()
{
  Log::get().notice("Stopping IPC socket...");
  acceptor_.cancel();
}



void IPC::start()
{
  Log::get().notice("Starting IPC socket...");
  ios_->run();
}



void IPC::handleAccept(boost::shared_ptr<IPCSession> session,
                       const boost::system::error_code& error)
{
  Log::get().notice("IPC connection accepted.");
  if (error)
  {
    std::cerr << error.message() << std::endl;
    return;
  }

  session->start();
  session.reset(new IPCSession(*ios_));
  acceptor_.async_accept(session->getSocket(),
                         boost::bind(&IPC::handleAccept, this, session,
                                     boost::asio::placeholders::error));
}
