#ifndef _ROUTER_H_
#define _ROUTER_H_

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <boost/url.hpp>
#include <list>
#include "../handlers/root_handler.h"

using handler = std::function<void(boost::beast::http::request<boost::beast::http::string_body>&&, boost::beast::http::response<boost::beast::http::string_body>&)>;

class Router{
private:
    const std::list<std::tuple<std::string,std::string,handler>> routes = {
        {"GET", "/", RootHandler::handleRequest},
    };

    std::vector<std::list<std::tuple<std::string,std::string,handler>>::const_iterator> findRouteByPath(const std::string &path);
public:
    boost::beast::http::message_generator handleRequest(boost::beast::http::request<boost::beast::http::string_body> &&request);
};

#endif