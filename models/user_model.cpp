#include "user_model.h"
#include "../db_client/db_client.h"
#include "../utils/logger.h"
#include <sodium.h>
#include <iomanip>

boost::asio::awaitable<UserModel::User> UserModel::getUserByUUID(const std::string &uuid){
    boost::mysql::results result;
    co_await DBClient::getConnection()->async_execute(boost::mysql::with_params("select uu_id,username,password_hash,email,created_at,updated_at from users where uu_id={}",uuid),result,boost::asio::use_awaitable);
    UserModel::User u;
    if(result.rows().size()){
        u.uu_id = result.rows()[0][0].as_string();
        u.username = result.rows()[0][1].as_string();
        u.password_hash = result.rows()[0][2].as_string();
        u.email = result.rows()[0][3].as_string();
        u.created_at = result.rows()[0][4].as_datetime();
        u.updated_at = result.rows()[0][5].as_datetime();
    } else {
        u.uu_id = "";
    }
    co_return u;
}

boost::asio::awaitable<void> UserModel::createUser(const std::string &username, const std::string &password, const std::string &email){
    char password_hash[crypto_pwhash_argon2id_STRBYTES];
    int err = crypto_pwhash_argon2id_str(password_hash,password.c_str(),password.size(),3U,100000000U);
    if(err) {
        throw std::runtime_error("crypto_pwhash_argon2id_str failed!");
    }

    boost::mysql::datetime createdAt = boost::mysql::datetime::now();

    boost::mysql::results result;
    co_await DBClient::getConnection()->async_execute(boost::mysql::with_params("insert into users values (UUID(),{0},{1},{2},{3},{4})",username,email,password_hash,createdAt,createdAt),result,boost::asio::use_awaitable);
    
    co_return;
}

boost::asio::awaitable<bool> UserModel::authenticateUser(const std::string &username, const std::string &password, const std::string &email){
    boost::mysql::results result;
    co_await DBClient::getConnection()->async_execute(boost::mysql::with_params("select password_hash from users where username={0} or email={1}",username,email),result,boost::asio::use_awaitable);
    
    std::string hashed_password = "";
    if(result.rows().size()){
        hashed_password = result.rows().at(0).at(0).as_string();
    }

    int res = crypto_pwhash_argon2id_str_verify(hashed_password.c_str(),password.c_str(),password.size());
    co_return res == 0;
}

boost::asio::awaitable<bool> UserModel::checkIfUserAlreadyExists(const std::string &username, const std::string &email){
    boost::mysql::results result;
    co_await DBClient::getConnection()->async_execute(boost::mysql::with_params("select username,email from users where username={0} or email={1}",username,email),result,boost::asio::use_awaitable);
    co_return result.rows().size();
}

void UserModel::tag_invoke(const boost::json::value_from_tag&, boost::json::value& jv, const User &user){
    jv = boost::json::object();
    jv.as_object()["uu_id"] = user.uu_id;
    jv.as_object()["username"] = user.username;
    jv.as_object()["password_hash"] = user.password_hash;
    jv.as_object()["email"] = user.email;

    std::chrono::microseconds createdAtMicro = user.created_at.as_time_point().time_since_epoch();
    std::time_t createdTime = createdAtMicro.count()/1000000;
    char createdAtTimestamp[200];
    strftime(createdAtTimestamp,sizeof(createdAtTimestamp),"%Y/%m/%d %H:%M:%S +00:00",std::gmtime(&createdTime));
    jv.as_object()["created_at"] = std::string(createdAtTimestamp);

    std::chrono::microseconds updatedAtMicro = user.updated_at.as_time_point().time_since_epoch();
    std::time_t updatedTime = updatedAtMicro.count()/1000000;
    char updatedAtTimestamp[200];
    strftime(updatedAtTimestamp,sizeof(updatedAtTimestamp),"%Y/%m/%d %H:%M:%S +00:00",std::gmtime(&updatedTime));
    jv.as_object()["updated_at"] = updatedAtTimestamp;
}
