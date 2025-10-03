#ifndef _USER_MODEL_H_
#define _USER_MODEL_H_

#include <string>
#include <ctime>
#include <boost/mysql.hpp>
#include <boost/asio.hpp>
#include <boost/json.hpp>

namespace UserModel {
    struct User {
        std::string uu_id;
        std::string username;
        std::string password_hash;
        std::string email;
        boost::mysql::datetime created_at;
        boost::mysql::datetime updated_at;
    };

    boost::asio::awaitable<User> getUserByUUID(const std::string &uuid);
    boost::asio::awaitable<int> createUser(const std::string &username, const std::string &password, const std::string &email);
    void tag_invoke(const boost::json::value_from_tag&, boost::json::value& jv, const User &user);
};


#endif