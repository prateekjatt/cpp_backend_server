#include "middlewares.h"
#include "../utils/cache_storage.h"
#include "../utils/logger.h"
#include "../utils/utilities.h"


RouteHandler Middlewares::authenticateRequest(RouteHandler next){

    std::function<boost::asio::awaitable<void>(HttpRequest&&,HttpResponse&,const RequestParams&)> fun = 
    [=](HttpRequest &&request, HttpResponse &response, const RequestParams &requestParams)->boost::asio::awaitable<void>{
        std::unordered_map<std::string,std::string> cookies = Utilities::parseCookies(request.find(boost::beast::http::field::cookie) != request.end()? request.at(boost::beast::http::field::cookie):"");
        if(!cookies.contains("Authentication") || cookies.at("Authentication").length() != 256 
        || std::find_if(cookies.at("Authentication").begin(),cookies.at("Authentication").end(),[](char &c){return !std::isalpha(c);}) != cookies.at("Authentication").end()){
            response.result(boost::beast::http::status::unauthorized);
            response.set(boost::beast::http::field::content_type,"application/json");
            boost::json::value resBody = {{"status","error"},{"message","Not Authorized!"}};
            response.body() = boost::json::serialize(resBody);
            co_return;
        }
        
        bool isTokenValid = co_await CacheStorage::getInstance()->has(cookies.at("Authentication"));
        if(!isTokenValid){
            response.result(boost::beast::http::status::unauthorized);
            response.set(boost::beast::http::field::content_type,"application/json");
            response.set(boost::beast::http::field::set_cookie,"Authentication=;Max-Age=0;Secure;Path=/;HttpOnly;");
            boost::json::value resBody = {{"status","error"},{"message","Session Expired!"}};
            response.body() = boost::json::serialize(resBody);
            co_return;
        }
        
        std::string username = co_await CacheStorage::getInstance()->get(cookies.at("Authentication"));
        
        RequestParams newRequestParams(requestParams);
        std::unordered_map<std::string,std::string> additionalParams;
        additionalParams["username"] = username;
        newRequestParams["additionalParams"] = additionalParams;
        
        co_await next(std::move(request),response,newRequestParams);
    };

    return fun;
}