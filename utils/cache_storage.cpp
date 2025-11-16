#include "./cache_storage.h"
#include "./configuration_manager.h"
#include "./logger.h"
#include "globals.h"

CacheStorage *CacheStorage::cacheStorage = nullptr;

CacheStorage::CacheStorage():conn(Globals::ioContext){
    const auto logger = Logger::getInstance();

    const std::string hostname = ConfigurationManager::get("CACHE_SERVER_HOSTNAME");
    const std::string port = ConfigurationManager::get("CACHE_SERVER_PORT");
    if(hostname.empty() || port.empty()){
        logger->log(LogType::ERROR,"Cache Server configurations not found!");
        exit(EXIT_FAILURE);
    }

    boost::redis::config config;
    config.addr.host = hostname;
    config.addr.port = port;

    conn.async_run(config,boost::redis::logger{boost::redis::logger::level::disabled},[logger](boost::system::error_code ec){
        if(ec){
            logger->log(LogType::ERROR,ec.what());
            exit(EXIT_FAILURE);
        } else {
            logger->log(LogType::INFO,"Cache Server Connected Successfully!");
        }
    });
}

CacheStorage* CacheStorage::getInstance(){
    if(cacheStorage == nullptr){
        cacheStorage = new CacheStorage();
    }
    return cacheStorage;
}

boost::asio::awaitable<void> CacheStorage::set(const std::string key, const std::string value, const std::string expiry){
    boost::redis::request req;
    boost::redis::response<std::string> res;
    req.push("SET",key,value,"EX",expiry);
    co_await conn.async_exec(req,res,boost::asio::use_awaitable);
}

boost::asio::awaitable<std::string> CacheStorage::get(const std::string key){
    boost::redis::request req;
    boost::redis::response<std::string> res;
    req.push("GET",key);
    co_await conn.async_exec(req,res,boost::asio::use_awaitable);
    co_return std::get<0>(res).value();
}

boost::asio::awaitable<void> CacheStorage::del(const std::string key){
    boost::redis::request req;
    boost::redis::response<bool> res;
    req.push("DEL",key);
    co_await conn.async_exec(req,res,boost::asio::use_awaitable);
}

boost::asio::awaitable<bool> CacheStorage::has(const std::string key){
    boost::redis::request req;
    boost::redis::response<int> res;
    req.push("EXISTS",key);
    co_await conn.async_exec(req,res,boost::asio::use_awaitable);
    co_return (std::get<0>(res).value() != 0);
}