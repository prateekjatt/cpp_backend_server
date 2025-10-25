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
    {"/users/signup",{ {boost::beast::http::verb::post, UserHandler::registerUser} }},
    {"/users/login",{ {boost::beast::http::verb::post, UserHandler::loginUser} }},
    {"/users/sendVerificationCode",{ {boost::beast::http::verb::post, UserHandler::sendVerificationCode} }},
    {"/users/{uu_id}",{ {boost::beast::http::verb::get, UserHandler::getUserByUUID} }},
    {"/users/checkUsername/{username}",{ {boost::beast::http::verb::get, UserHandler::checkIfUsernameExists} }},
    {"/users/checkEmail/{email}",{ {boost::beast::http::verb::get, UserHandler::checkIfEmailExists} }},
};

#endif