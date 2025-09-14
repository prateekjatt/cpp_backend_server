#include "server.h"
#include "http_session.h"
#include "../db_client/db_client.h"


AppServer::AppServer(const std::string &hostname,const uint16_t &port) : hostname(hostname),port(port),acceptor(ioContext){
    acceptor.open(tcp::v4());
    acceptor.bind(tcp::endpoint{boost::asio::ip::make_address(hostname),port});
    acceptor.set_option(boost::asio::socket_base::reuse_address(true));
    acceptor.listen();
}

void AppServer::start(){
    std::cout << "INFO: Server listening on " << hostname << ":" << port << std::endl;
    doAccept();
    ioContext.run();
}

void AppServer::doAccept(){
    acceptor.async_accept(ioContext,boost::beast::bind_front_handler(&AppServer::onAccept,shared_from_this()));
}

void AppServer::onAccept(boost::beast::error_code ec,tcp::socket socket){
    if(ec){
        std::cout << "ERROR: While Accepting Request: " << ec.message() << std::endl;
        return;
    }

    std::make_shared<HttpSession>(std::move(socket))->start();

    doAccept();
}

AppServer::~AppServer(){
    std::cout << "INFO: Closing Server" << std::endl;
}