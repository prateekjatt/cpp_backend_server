#ifndef _APPSERVER_H_
#define _APPSERVER_H_

#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

class AppServer : public std::enable_shared_from_this<AppServer>{
    using tcp = boost::asio::ip::tcp;
private:
    uint16_t port;
    std::string hostname;
    boost::asio::io_context ioContext;
    tcp::acceptor acceptor;
public:
    AppServer(const std::string &hostname,const uint16_t &port);
    void start();
    void doAccept();
    void onAccept(boost::beast::error_code ec,tcp::socket socket);
    ~AppServer();
};

#endif