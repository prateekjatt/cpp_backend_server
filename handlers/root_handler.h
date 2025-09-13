#ifndef _ROOT_HANDLER_H_
#define _ROOT_HANDLER_H_

#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

class RootHandler{
private:
public:
    RootHandler();
    boost::beast::http::message_generator handleRequest(boost::beast::http::request<boost::beast::http::string_body> &&request);
    ~RootHandler();
};

#endif