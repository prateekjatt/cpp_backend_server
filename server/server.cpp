#include "server.h"
#include "http_session.h"


AppServer::AppServer(const uint64_t &port) : port(port),acceptor(ioContext){
    acceptor.open(tcp::v4());
    acceptor.bind(tcp::endpoint(tcp::v4(),port));
}

void AppServer::start(){
    acceptor.listen();
    doAccept();
    ioContext.run();
}

void AppServer::doAccept(){
    acceptor.async_accept(ioContext,boost::beast::bind_front_handler(&AppServer::onAccept,shared_from_this()));
}

void AppServer::onAccept(boost::beast::error_code ec,tcp::socket socket){
    if(ec){
        std::cout << "Error Accepting Request: " << ec.to_string() << std::endl;
        return;
    }

    std::make_shared<HttpSession>(std::move(socket))->readRequest();


    doAccept();
}

AppServer::~AppServer(){
}