#include "server.h"

AppServer::AppServer(boost::asio::io_context &ioContext,const std::string &hostname,const uint16_t &port) : hostname(hostname),port(port),acceptor(ioContext){
    logger = Logger::getInstance();
    acceptor.open(tcp::v4());
    acceptor.bind(tcp::endpoint{boost::asio::ip::make_address(hostname),port});
    acceptor.set_option(boost::asio::socket_base::reuse_address(true));
    acceptor.listen();
}

void AppServer::start(){
    logger->log(LogType::INFO,"Server listening on " + hostname + ":" + std::to_string(port));
    doAccept();
}

void AppServer::doAccept(){
    acceptor.async_accept(boost::beast::bind_front_handler(&AppServer::onAccept,shared_from_this()));
}

void AppServer::onAccept(boost::beast::error_code ec,tcp::socket socket){
    if(ec){
        logger->log(LogType::ERROR,"While Accepting Request: " + ec.what());
        return;
    }

    std::make_shared<HttpSession>(std::move(socket),router)->start();

    doAccept();
}

AppServer::~AppServer(){
    logger->log(LogType::INFO,"Closing Server");
}