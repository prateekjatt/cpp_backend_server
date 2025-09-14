#ifndef _DB_CLIENT_H_
#define _DB_CLIENT_H_

#include <iostream>
#include <boost/asio.hpp>
#include <boost/mysql.hpp>

class DBClient{
private:
    std::string hostname;
    uint16_t port;
    std::string username;
    std::string password;
    boost::mysql::any_connection conn;
public:
    DBClient(boost::asio::io_context &ioContext);
    void connect();
    void execute();
    void disconnect();
    ~DBClient();
};

#endif