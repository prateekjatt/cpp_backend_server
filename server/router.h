#ifndef _ROUTER_H_
#define _ROUTER_H_

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <boost/url.hpp>
#include <vector>
#include "../utils/logger.h"
#include "../handlers/type_alias.h"

struct SegementedRoute{
    std::vector<std::string> path; 
    std::unordered_map<std::string,RouteHandler> methodHandlers;
};

class Router{
private:    
    std::vector<SegementedRoute> segementedRoutes;
    Logger *logger;
    bool findRouteByPath(const boost::urls::segments_view &path,const std::string &method,std::vector<std::pair<std::string,std::string>> &pathParams,RouteHandler &handler);
public:
    Router();
    boost::asio::awaitable<boost::beast::http::message_generator> handleRequest(HttpRequest &&request);
};

#endif