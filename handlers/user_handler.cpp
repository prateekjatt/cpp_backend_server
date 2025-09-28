#include "user_handler.h"
#include "../db_client/db_client.h"

boost::asio::awaitable<void> UserHandler::createUser(HttpRequest &&request, HttpResponse &response,const RequestParams &requestParams){
    
    boost::system::error_code ec;
    boost::json::value reqBody = boost::json::parse(request.body(),ec);
    if(ec){
        response.result(boost::beast::http::status::bad_request);
        response.set(boost::beast::http::field::content_type,"application/json");

        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "error";
        res.as_object()["message"] = "Bad Request!";
        
        response.body() = boost::json::serialize(res);
        co_return;
    }
    
    if(!reqBody.get_object().contains("username") || !reqBody.get_object().contains("password") || !reqBody.get_object().contains("email")){
        response.result(boost::beast::http::status::unprocessable_entity);
        response.set(boost::beast::http::field::content_type,"application/json");
    
        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "error";
        res.as_object()["message"] = "Required Fields not present in body!";
        
        response.body() = boost::json::serialize(res);
        co_return;
    }
    if(!reqBody.get_object().at("username").is_string() || !reqBody.get_object().at("password").is_string() || !reqBody.get_object().at("email").is_string()){
        response.result(boost::beast::http::status::unprocessable_entity);
        response.set(boost::beast::http::field::content_type,"application/json");
    
        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "error";
        res.as_object()["message"] = "Invalid Field Types!";
        
        response.body() = boost::json::serialize(res);
        co_return;
    }
    if(reqBody.get_object().at("username").get_string() == "" || reqBody.get_object().at("password").get_string() == "" || reqBody.get_object().at("email").get_string() == ""){
        response.result(boost::beast::http::status::unprocessable_entity);
        response.set(boost::beast::http::field::content_type,"application/json");
    
        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "error";
        res.as_object()["message"] = "Required Fields cannot be empty!";
        
        response.body() = boost::json::serialize(res);
        co_return;
    }
    
    response.result(boost::beast::http::status::ok);
    response.set(boost::beast::http::field::content_type,"application/json");
    boost::json::value res = boost::json::object();
    res.as_object()["status"] = "success";
    res.as_object()["message"] = "User Created Successfully!";
    response.body() = boost::json::serialize(res);
    co_return;
}