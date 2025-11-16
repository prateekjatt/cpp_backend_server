#ifndef _DB_CLIENT_H_
#define _DB_CLIENT_H_

#include <boost/asio.hpp>
#include <boost/mysql.hpp>
#include "../utils/logger.h"

class DBClient{
private:
    boost::mysql::connection_pool conn_pool;
    Logger *logger;
    static DBClient *dbClient;
    DBClient();
public:
    static DBClient* getInstance();
    static boost::asio::awaitable<boost::mysql::pooled_connection> getConnection();
    void disconnect();
    ~DBClient();
};

#endif