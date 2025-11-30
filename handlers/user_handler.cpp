#include "user_handler.h"
#include "../models/user_model.h"
#include "../utils/cache_storage.h"
#include "../utils/email_service.h"
#include "../utils/logger.h"
#include "../utils/utilities.h"

boost::asio::awaitable<void> UserHandler::registerUser(HttpRequest &&request, HttpResponse &response, const RequestParams &requestParams){
    
    boost::system::error_code ec;
    boost::json::value reqBody = boost::json::parse(request.body(),ec);
    if(ec){
        response.result(boost::beast::http::status::bad_request);
        response.set(boost::beast::http::field::content_type,"application/json");

        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "error";
        res.as_object()["message"] = "Bad Request!";
        
        response.body() = boost::json::serialize(res);
        co_return;
    }
    
    if(!reqBody.get_object().contains("username") || !reqBody.get_object().contains("password") || 
    !reqBody.get_object().contains("email") || 
    !reqBody.get_object().contains("verificationCode")){
        response.result(boost::beast::http::status::unprocessable_entity);
        response.set(boost::beast::http::field::content_type,"application/json");
    
        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "error";
        res.as_object()["message"] = "Required Fields not present in body!";
        
        response.body() = boost::json::serialize(res);
        co_return;
    }

    const boost::json::value username = reqBody.get_object().at("username"); 
    const boost::json::value password = reqBody.get_object().at("password"); 
    const boost::json::value email = reqBody.get_object().at("email"); 
    const boost::json::value verificationCode = reqBody.get_object().at("verificationCode"); 
    if(!username.is_string() || !password.is_string() || !email.is_string() || !verificationCode.is_string()){
        response.result(boost::beast::http::status::unprocessable_entity);
        response.set(boost::beast::http::field::content_type,"application/json");
    
        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "error";
        res.as_object()["message"] = "Invalid Fields Type!";
        
        response.body() = boost::json::serialize(res);
        co_return;
    }
    if(username.get_string().empty() || password.get_string().empty() || email.get_string().empty() || 
    verificationCode.get_string().empty()){
        response.result(boost::beast::http::status::unprocessable_entity);
        response.set(boost::beast::http::field::content_type,"application/json");
    
        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "error";
        res.as_object()["message"] = "Required Fields cannot be empty!";
        
        response.body() = boost::json::serialize(res);
        co_return;
    }

    bool usernameAlreadyExists = co_await UserModel::checkIfUsernameAlreadyExists(username.get_string().c_str());
    bool emailAlreadyExists = co_await UserModel::checkIfEmailAlreadyExists(email.get_string().c_str());
    if(usernameAlreadyExists || emailAlreadyExists){
        response.result(boost::beast::http::status::ok);
        response.set(boost::beast::http::field::content_type,"application/json");
    
        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "error";
        res.as_object()["message"] = "Username or Email already exists!";
        
        response.body() = boost::json::serialize(res);
        co_return;
    }
    
    bool verificationCodeValid = co_await CacheStorage::getInstance()->has("otp:"+std::string(email.get_string()));
    std::string verificationCodeSent = "";
    if(verificationCodeValid){
        verificationCodeSent = co_await CacheStorage::getInstance()->get("otp:"+std::string(email.get_string()));
    }
    if(verificationCodeSent != verificationCode.get_string()){
        response.result(boost::beast::http::status::ok);
        response.set(boost::beast::http::field::content_type,"application/json");
    
        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "error";
        res.as_object()["message"] = "Verification Code Invalid!";
        
        response.body() = boost::json::serialize(res);
        co_return;   
    }
    
    co_await CacheStorage::getInstance()->del("otp:"+std::string(email.get_string()));

    co_await UserModel::createUser(username.get_string().c_str(),password.get_string().c_str(),email.get_string().c_str());

    std::unordered_map<std::string,std::string> emailData({{"username",username.get_string().c_str()}});
    co_await EmailService::sendEmail(email.get_string().c_str(),EmailService::EmailType::WELCOME_EMAIL,emailData);

    response.result(boost::beast::http::status::ok);
    response.set(boost::beast::http::field::content_type,"application/json");
    boost::json::value res = boost::json::object();
    res.as_object()["status"] = "success";
    res.as_object()["message"] = "User Created Successfully!";
    response.body() = boost::json::serialize(res);
    co_return;
}

boost::asio::awaitable<void> UserHandler::loginUser(HttpRequest &&request, HttpResponse &response, const RequestParams &requestParams){
    
    boost::system::error_code ec;
    boost::json::value reqBody = boost::json::parse(request.body(),ec);
    if(ec){
        response.result(boost::beast::http::status::bad_request);
        response.set(boost::beast::http::field::content_type,"application/json");

        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "error";
        res.as_object()["message"] = "Bad Request!";
        
        response.body() = boost::json::serialize(res);
        co_return;
    }
    
    if((!reqBody.get_object().contains("username") && !reqBody.get_object().contains("email")) || 
    !reqBody.get_object().contains("password")){
        response.result(boost::beast::http::status::unprocessable_entity);
        response.set(boost::beast::http::field::content_type,"application/json");
    
        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "error";
        res.as_object()["message"] = "Required Fields not present in body!";
        
        response.body() = boost::json::serialize(res);
        co_return;
    }


    const boost::json::value password = reqBody.get_object().at("password"); 
    std::string usernameString = "";
    if(reqBody.get_object().contains("username")){
        const boost::json::value username = reqBody.get_object().at("username");
        if(!username.is_string() || !password.is_string()){
            response.result(boost::beast::http::status::unprocessable_entity);
            response.set(boost::beast::http::field::content_type,"application/json");
        
            boost::json::value res = boost::json::object();
            res.as_object()["status"] = "error";
            res.as_object()["message"] = "Invalid Fields Type!";
            
            response.body() = boost::json::serialize(res);
            co_return;
        }
        usernameString = username.get_string();
    }
    else {
        const boost::json::value email = reqBody.get_object().at("email");
        if(!email.is_string() || !password.is_string()){
            response.result(boost::beast::http::status::unprocessable_entity);
            response.set(boost::beast::http::field::content_type,"application/json");
        
            boost::json::value res = boost::json::object();
            res.as_object()["status"] = "error";
            res.as_object()["message"] = "Invalid Fields Type!";
            
            response.body() = boost::json::serialize(res);
            co_return;
        }
        UserModel::User user = co_await UserModel::getUserByEmail(email.get_string().c_str());
        if(!user.username.empty()){
            usernameString = user.username;
        }
    }

    bool success = co_await UserModel::authenticateUser(usernameString,password.get_string().c_str());
    if(success) {
        std::string authToken = "";
        std::string alphabets = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        srand(time(NULL));
        for(int i=0;i<256;i++){
            authToken += alphabets[rand()%52];
        }

        co_await CacheStorage::getInstance()->set(authToken,usernameString,"86400");
        
        response.set(boost::beast::http::field::set_cookie,"Authentication="+authToken+";Max-Age="+std::to_string(24*60*60)+";Secure;Path=/;HttpOnly;");

        response.result(boost::beast::http::status::ok);
        response.set(boost::beast::http::field::content_type,"application/json");
        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "success";
        res.as_object()["message"] = "User Login Successfull!";
        response.body() = boost::json::serialize(res);
    } else {
        response.result(boost::beast::http::status::ok);
        response.set(boost::beast::http::field::content_type,"application/json");
        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "error";
        res.as_object()["message"] = "Invalid Credentials!";
        response.body() = boost::json::serialize(res);
    }
    co_return;
}

boost::asio::awaitable<void> UserHandler::logoutUser(HttpRequest &&request, HttpResponse &response, const RequestParams &requestParams){
    std::unordered_map<std::string,std::string> cookies = Utilities::parseCookies(request.find(boost::beast::http::field::cookie) != request.end()? request.at(boost::beast::http::field::cookie):"");
        
    if(cookies.contains("Authentication")){
        co_await CacheStorage::getInstance()->del(cookies.at("Authentication"));
    }
    
    response.set(boost::beast::http::field::set_cookie,"Authentication=;Max-Age=0;Secure;Path=/;HttpOnly;");

    response.result(boost::beast::http::status::ok);
    response.set(boost::beast::http::field::content_type,"application/json");
    boost::json::value res = boost::json::object();
    res.as_object()["status"] = "success";
    res.as_object()["message"] = "User Logged Out!";
    response.body() = boost::json::serialize(res);

    co_return;
}

boost::asio::awaitable<void> UserHandler::getUserByUUID(HttpRequest &&request, HttpResponse &response, const RequestParams &requestParams){

    UserModel::User u = co_await UserModel::getUserByUUID(requestParams.at("pathParams").at("uu_id"));

    if(u.uu_id.empty()){
        response.result(boost::beast::http::status::ok);
        response.set(boost::beast::http::field::content_type,"application/json");
        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "error";
        res.as_object()["message"] = "User Not Found!";
        response.body() = boost::json::serialize(res);
    } else {
        response.result(boost::beast::http::status::ok);
        response.set(boost::beast::http::field::content_type,"application/json");
        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "success";
        res.as_object()["user"] = boost::json::value_from<UserModel::User&>(u);
        res.as_object()["user"].as_object().erase("password_hash");
        response.body() = boost::json::serialize(res);
    }
    
    co_return;
}

boost::asio::awaitable<void> UserHandler::getCurrentUser(HttpRequest &&request, HttpResponse &response, const RequestParams &requestParams){

    UserModel::User u = co_await UserModel::getUserByUsername(requestParams.at("additionalParams").at("username"));

    if(u.uu_id.empty()){
        response.result(boost::beast::http::status::ok);
        response.set(boost::beast::http::field::content_type,"application/json");
        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "error";
        res.as_object()["message"] = "User Not Found!";
        response.body() = boost::json::serialize(res);
    } else {
        response.result(boost::beast::http::status::ok);
        response.set(boost::beast::http::field::content_type,"application/json");
        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "success";
        res.as_object()["user"] = boost::json::value_from<UserModel::User&>(u);
        res.as_object()["user"].as_object().erase("password_hash");
        res.as_object()["user"].as_object().erase("uu_id");
        res.as_object()["user"].as_object().erase("created_at");
        res.as_object()["user"].as_object().erase("updated_at");
        response.body() = boost::json::serialize(res);
    }
    
    co_return;
}

boost::asio::awaitable<void> UserHandler::checkIfUsernameExists(HttpRequest &&request, HttpResponse &response, const RequestParams &requestParams){

    bool usernameExists = co_await UserModel::checkIfUsernameAlreadyExists(requestParams.at("pathParams").at("username"));

    response.result(boost::beast::http::status::ok);
    response.set(boost::beast::http::field::content_type,"application/json");
    boost::json::value res = boost::json::object();
    res.as_object()["status"] = "success";
    res.as_object()["usernameExists"] = usernameExists;
    response.body() = boost::json::serialize(res);
    co_return;
}

boost::asio::awaitable<void> UserHandler::checkIfEmailExists(HttpRequest &&request, HttpResponse &response, const RequestParams &requestParams){

    bool emailExists = co_await UserModel::checkIfEmailAlreadyExists(requestParams.at("pathParams").at("email"));

    response.result(boost::beast::http::status::ok);
    response.set(boost::beast::http::field::content_type,"application/json");
    boost::json::value res = boost::json::object();
    res.as_object()["status"] = "success";
    res.as_object()["emailExists"] = emailExists;
    response.body() = boost::json::serialize(res);
    co_return;
}

boost::asio::awaitable<void> UserHandler::sendVerificationCode(HttpRequest &&request, HttpResponse &response, const RequestParams &requestParams){
    
    boost::system::error_code ec;
    boost::json::value reqBody = boost::json::parse(request.body(),ec);
    if(ec){
        response.result(boost::beast::http::status::bad_request);
        response.set(boost::beast::http::field::content_type,"application/json");

        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "error";
        res.as_object()["message"] = "Bad Request!";
        
        response.body() = boost::json::serialize(res);
        co_return;
    }

    const boost::json::value email = reqBody.get_object().at("email");
    if(!email.is_string()){
        response.result(boost::beast::http::status::unprocessable_entity);
        response.set(boost::beast::http::field::content_type,"application/json");
    
        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "error";
        res.as_object()["message"] = "Invalid Fields Type!";
        
        response.body() = boost::json::serialize(res);
        co_return;
    }
    if(email.get_string().empty()){
        response.result(boost::beast::http::status::unprocessable_entity);
        response.set(boost::beast::http::field::content_type,"application/json");
    
        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "error";
        res.as_object()["message"] = "Email cannot be empty!";
        
        response.body() = boost::json::serialize(res);
        co_return;
    }
    
    std::string otp = "";
    srand(time(NULL));
    for(int i=0;i<6;i++){
        otp += std::to_string(rand()%10);
    }

    co_await CacheStorage::getInstance()->set(std::string("otp:")+email.as_string().c_str(),otp,"1800");

    std::unordered_map<std::string,std::string> emailData({{"OTP",otp}});
    co_await EmailService::sendEmail(email.as_string().c_str(), EmailService::EmailType::SIGNUP_OTP_EMAIL,emailData);

    response.result(boost::beast::http::status::ok);
    response.set(boost::beast::http::field::content_type,"application/json");
    boost::json::value res = boost::json::object();
    res.as_object()["status"] = "success";
    res.as_object()["message"] = "OTP Sent!";
    response.body() = boost::json::serialize(res);
    
    co_return;
}
