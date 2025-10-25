#ifndef _CACHE_STORAGE_H_
#define _CACHE_STORAGE_H_

#include <boost/redis/connection.hpp>
#include <boost/asio.hpp>
#include<string>

class CacheStorage {
private:
    boost::redis::connection conn;
    static CacheStorage *cacheStorage;
    CacheStorage(boost::asio::io_context &ioContext);
public:
    static void createInstance(boost::asio::io_context &ioContext);
    static CacheStorage* getInstance();
    boost::asio::awaitable<void> set(const std::string key,const std::string value,const std::string expiry);
    boost::asio::awaitable<std::string> get(const std::string key);
    boost::asio::awaitable<void> del(const std::string key);
    boost::asio::awaitable<bool> has(const std::string key);
};


#endif