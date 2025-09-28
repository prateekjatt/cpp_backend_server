#ifndef _ROUTES_H_
#define _ROUTES_H_

#include <vector>
#include <unordered_map>
#include "../handlers/type_alias.h"
#include "../handlers/user_handler.h"

struct Route{
    std::string path; 
    std::unordered_map<boost::beast::http::verb,RouteHandler> methodHandlers;
};

static std::vector<Route> routes = {
    {"/signup",{ {boost::beast::http::verb::post, UserHandler::createUser} }},
};

#endif