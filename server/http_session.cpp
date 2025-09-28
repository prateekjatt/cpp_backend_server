#include "http_session.h"

HttpSession::HttpSession(tcp::socket &&socket,const Router &router):stream(std::move(socket)),router(router){
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
        closeSession();
        return;
    }
    if(ec){
        logger->log(LogType::ERROR,"While Reading Request: " + ec.what());
        return;
    }

    boost::asio::co_spawn(stream.get_executor(),[self=shared_from_this()]() -> boost::asio::awaitable<void> {

        self->sendResponse(co_await self->router.handleRequest(std::move(self->request)));
        
    }, boost::asio::detached);
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
        closeSession();
        return;
    }
    readRequest();
}

void HttpSession::closeSession(){
    boost::beast::error_code ec;
    stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both,ec);
}

HttpSession::~HttpSession(){
}