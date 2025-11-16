#ifndef _MIDDLEWARES_H_
#define _MIDDLEWARES_H_

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include "type_alias.h"

namespace Middlewares{
    RouteHandler authenticateRequest(RouteHandler next);
};

#endif