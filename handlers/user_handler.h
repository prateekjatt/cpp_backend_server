#ifndef _USER_HANDLER_H_
#define _USER_HANDLER_H_

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include "type_alias.h"

namespace UserHandler{
    boost::asio::awaitable<void> createUser(HttpRequest &&request, HttpResponse &response,const RequestParams &requestParams);
};

#endif