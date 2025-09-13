#include "root_handler.h"

RootHandler::RootHandler(){
}

boost::beast::http::message_generator RootHandler::handleRequest(boost::beast::http::request<boost::beast::http::string_body> &&request){
    if(request.method() == boost::beast::http::verb::get && request.target() == "/"){
        boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::ok,request.version()};
        res.set(boost::beast::http::field::content_type,"text/html");
        res.body() = "<h1>Web Server Using Boost C++!</h1>";
        return res;
    }
    return boost::beast::http::response<boost::beast::http::empty_body>(boost::beast::http::status::not_found,request.version());
}

RootHandler::~RootHandler(){
}