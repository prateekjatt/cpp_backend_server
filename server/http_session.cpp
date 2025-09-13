#include "http_session.h"

HttpSession::HttpSession(tcp::socket &&socket):stream(std::move(socket)){
}

void HttpSession::readRequest(){
    request = {};
    stream.expires_after(std::chrono::seconds(30));
    boost::beast::http::async_read(stream,buff,request,boost::beast::bind_front_handler(&HttpSession::onRead,shared_from_this()));
}

void HttpSession::onRead(boost::beast::error_code ec,std::size_t bytes_transferred){
    if(ec == boost::beast::http::error::end_of_stream){
        return closeSession();
    }
    if(ec){
        std::cout << "Error Reading Request: " << ec.to_string() << std::endl;
        return;
    }

    // Write Logic for handling http request and sending response to client
}

void HttpSession::writeResponse(){

}

void HttpSession::closeSession(){
    boost::beast::error_code ec;
    stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send,ec);
}

HttpSession::~HttpSession(){
}