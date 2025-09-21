#include "http_session.h"
#include "router.h"

HttpSession::HttpSession(tcp::socket &&socket):stream(std::move(socket)){
    logger = Logger::getInstance();
}

void HttpSession::start(){
    readRequest();
}

void HttpSession::readRequest(){
    request = {};
    stream.expires_after(std::chrono::seconds(30));
    boost::beast::http::async_read(stream,buff,request,boost::beast::bind_front_handler(&HttpSession::onRead,shared_from_this()));
}

void HttpSession::onRead(boost::beast::error_code ec,std::size_t bytes_transferred){
    if(ec == boost::beast::error::timeout || ec == boost::beast::http::error::end_of_stream){
        return closeSession();
    }
    if(ec){
        logger->log(LogType::ERROR,"While Reading Request: " + ec.what());
        return;
    }

    logger->log(LogType::INFO, std::string(request.method_string()) + " " + std::string(request.target()) + " HTTP/" + std::to_string(request.version()/10) + "." + std::to_string(request.version()%10));

    Router router;
    sendResponse(router.handleRequest(std::move(request)));
}

void HttpSession::sendResponse(boost::beast::http::message_generator &&msg){
    bool keep_alive = msg.keep_alive();
    boost::beast::async_write(stream,std::move(msg),boost::beast::bind_front_handler(&HttpSession::onSend,shared_from_this(),keep_alive));
}

void HttpSession::onSend(bool keep_alive,boost::beast::error_code ec,std::size_t bytes_transferred){
    if(ec){
        logger->log(LogType::ERROR,"While Sending Response: " + ec.what());
        return;
    }
    if(!keep_alive){
        return closeSession();
    }
    readRequest();
}

void HttpSession::closeSession(){
    boost::beast::error_code ec;
    stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both,ec);
}

HttpSession::~HttpSession(){
}