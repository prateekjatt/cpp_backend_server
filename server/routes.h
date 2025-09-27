#ifndef _ROUTES_H_
#define _ROUTES_H_

#include <vector>
#include <unordered_map>
#include "../handlers/type_alias.h"
#include "../handlers/root_handler.h"

struct Route{
    std::string path; 
    std::unordered_map<std::string,RouteHandler> methodHandlers;
};

static std::vector<Route> routes = {
    {"/{user}",{ {"GET", RootHandler::handleRequest} }},
};

#endif