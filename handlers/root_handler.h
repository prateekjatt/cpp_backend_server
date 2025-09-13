#ifndef _ROOT_HANDLER_H_
#define _ROOT_HANDLER_H_

#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

class RootHandler : public std::enable_shared_from_this<RootHandler>{
    using tcp = boost::asio::ip::tcp;
private:
    tcp::socket socket;
public:
    RootHandler(tcp::socket &&socket);
    void handleRequest();
    ~RootHandler();
};

#endif