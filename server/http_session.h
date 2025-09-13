#ifndef _HTTP_SESSION_H_
#define _HTTP_SESSION_H_

#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

class HttpSession : public std::enable_shared_from_this<HttpSession>{
    using tcp = boost::asio::ip::tcp;
private:
    boost::beast::tcp_stream stream;
    boost::beast::flat_buffer buff;
    boost::beast::http::request<boost::beast::http::string_body> request;
public:
    HttpSession(tcp::socket &&socket);
    void readRequest();
    void onRead(boost::beast::error_code ec,std::size_t bytes_transferred);
    void writeResponse();
    void closeSession();
    ~HttpSession();
};

#endif