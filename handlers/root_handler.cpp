#include "root_handler.h"

void RootHandler::handleRequest(HttpRequest &&request, HttpResponse &response,const RequestParams &requestParams){
    response.result(boost::beast::http::status::ok);
    response.set(boost::beast::http::field::content_type,"application/json");
    
    boost::json::value res = boost::json::object();
    res.as_object()["status"] = "success";
    res.as_object()["message"] = "Web Server Using Boost C++!";
    res.as_object()["pathParams"] = requestParams.at("pathParams").at("user");
    res.as_object()["queryParams"] = requestParams.at("queryParams").find("query") != requestParams.at("queryParams").end()? requestParams.at("queryParams").at("query"):"";

    response.body() = boost::json::serialize(res);
}