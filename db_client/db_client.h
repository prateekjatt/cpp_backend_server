#ifndef _DB_CLIENT_H_
#define _DB_CLIENT_H_

#include <boost/asio.hpp>
#include <boost/mysql.hpp>
#include "../utils/logger.h"

class DBClient{
private:
    std::string hostname;
    uint16_t port;
    std::string username;
    std::string password;
    std::string database;
    std::shared_ptr<boost::mysql::any_connection> conn;
    Logger *logger;
    static DBClient *dbClient;
    DBClient(boost::asio::io_context &ioContext);
public:
    static void createInstance(boost::asio::io_context &ioContext);
    static DBClient* getInstance();
    static std::shared_ptr<boost::mysql::any_connection> getConnection();
    void connect();
    void disconnect();
    ~DBClient();
};

#endif