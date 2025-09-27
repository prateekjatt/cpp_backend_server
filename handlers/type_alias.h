#ifndef _TYPE_ALIAS_H_
#define _TYPE_ALIAS_H_

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <unordered_map>

using HttpRequest = boost::beast::http::request<boost::beast::http::string_body>;
using HttpResponse = boost::beast::http::response<boost::beast::http::string_body>;
using RequestParams = std::unordered_map<std::string,std::unordered_map<std::string,std::string>>;
using RouteHandler = std::function<void(HttpRequest&&, HttpResponse&, const RequestParams&)>;


#endif