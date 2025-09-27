#ifndef _ROOT_HANDLER_H_
#define _ROOT_HANDLER_H_

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include "type_alias.h"

namespace RootHandler{
    void handleRequest(HttpRequest &&request, HttpResponse &response,const RequestParams &requestParams);
};

#endif