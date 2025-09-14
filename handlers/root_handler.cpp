#include "root_handler.h"

RootHandler::RootHandler(){
}

boost::beast::http::message_generator RootHandler::handleRequest(boost::beast::http::request<boost::beast::http::string_body> &&request){
    boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::ok,request.version()};
    res.keep_alive(request.keep_alive());
    if(request.method() == boost::beast::http::verb::get && request.target() == "/"){
        res.set(boost::beast::http::field::content_type,"text/html");
        res.body() = "<h1>Web Server Using Boost C++!</h1>";
    } else if(request.method() == boost::beast::http::verb::post && request.target() == "/"){
        boost::system::error_code ec;
        boost::json::value requestJson = boost::json::parse(request.body(),ec);
        if(ec){
            res.result(boost::beast::http::status::bad_request);
            res.set(boost::beast::http::field::content_type,"text/html");
            res.body() = "<h1>Error Parsing Json</h1>";
        } else {
            res.set(boost::beast::http::field::content_type,"application/json");
            if(requestJson.as_object()["request"] == "status"){
                requestJson.as_object().clear();
                requestJson.as_object()["status"] = "Server Working Fine!";
            } else {
                requestJson.as_object().clear();
                requestJson.as_object()["error"] = "Invalid Request!";
            }
            res.body() = boost::json::serialize(requestJson);

        }
    }
    res.prepare_payload();
    return res;
}

RootHandler::~RootHandler(){
}