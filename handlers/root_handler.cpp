#include "root_handler.h"

void RootHandler::handleRequest(boost::beast::http::request<boost::beast::http::string_body> &&request, boost::beast::http::response<boost::beast::http::string_body> &response){
    response.result(boost::beast::http::status::ok);
    response.set(boost::beast::http::field::content_type,"text/html");
    response.body() = "<h1>Web Server Using Boost C++!</h1>";


    // if(request.method() == boost::beast::http::verb::post && request.target() == "/"){
    //     res.result(boost::beast::http::status::ok);
    //     boost::system::error_code ec;
    //     boost::json::value requestJson = boost::json::parse(request.body(),ec);
    //     if(ec){
    //         res.result(boost::beast::http::status::bad_request);
    //         res.set(boost::beast::http::field::content_type,"text/html");
    //         res.body() = "<h1>Error Parsing Json</h1>";
    //     } else {
    //         res.set(boost::beast::http::field::content_type,"application/json");
    //         if(requestJson.as_object()["request"] == "status"){
    //             requestJson.as_object().clear();
    //             requestJson.as_object()["status"] = "Server Working Fine!";
    //         } else {
    //             requestJson.as_object().clear();
    //             requestJson.as_object()["error"] = "Invalid Request!";
    //         }
    //         res.body() = boost::json::serialize(requestJson);

    //     }
    // }
}