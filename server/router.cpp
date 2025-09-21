#include "router.h"

boost::beast::http::message_generator Router::handleRequest(boost::beast::http::request<boost::beast::http::string_body> &&request){
    boost::beast::http::response<boost::beast::http::string_body> response{boost::beast::http::status::not_found,request.version()};
    response.keep_alive(request.keep_alive());
    
    auto parsed_url = boost::urls::parse_origin_form(request.target());
    if(parsed_url.has_error()){
        response.result(boost::beast::http::status::bad_request);
        response.set(boost::beast::http::field::content_type,"text/html");
        response.body() = "<h1>400 - Bad Request!</h1>";
        response.prepare_payload();
        return response;
    }
    
    auto itrs = findRouteByPath(parsed_url.value().path());
    if(itrs.size() == 0){
        response.set(boost::beast::http::field::content_type,"text/html");
        response.body() = "<h1>404 - Not Found!</h1>";
        response.prepare_payload();
        return response;
    }
    
    std::string method = request.method_string();
    auto itr = std::find_if(itrs.begin(),itrs.end(),[&method](const auto &itr){
        return (std::get<0>(*itr) == method);
    });
    if(itr == itrs.end()) {
        response.result(boost::beast::http::status::method_not_allowed);
        response.set(boost::beast::http::field::content_type,"text/html");
        response.body() = "<h1>405 - Method Not Allowed!</h1>";
        response.prepare_payload();
        return response;
    }

    (std::get<2>(**itr))(std::move(request),response);

    response.prepare_payload();
    return response;
}

std::vector<std::list<std::tuple<std::string,std::string,handler>>::const_iterator> Router::findRouteByPath(const std::string &path) {
    // Implement Route Matching
    std::vector<std::list<std::tuple<std::string,std::string,handler>>::const_iterator> matchedRoutes;
    matchedRoutes.push_back(routes.begin());
    return matchedRoutes;
}