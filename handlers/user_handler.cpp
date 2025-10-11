#include "user_handler.h"
#include "../models/user_model.h"

boost::asio::awaitable<void> UserHandler::registerUser(HttpRequest &&request, HttpResponse &response,const RequestParams &requestParams){
    
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
    
    if(!reqBody.get_object().contains("username") || !reqBody.get_object().contains("password") || !reqBody.get_object().contains("email")){
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
    if(!username.is_string() || !password.is_string() || !email.is_string()){
        response.result(boost::beast::http::status::unprocessable_entity);
        response.set(boost::beast::http::field::content_type,"application/json");
    
        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "error";
        res.as_object()["message"] = "Invalid Fields Type!";
        
        response.body() = boost::json::serialize(res);
        co_return;
    }
    if(username.get_string().empty() || password.get_string().empty() || email.get_string().empty()){
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
    
    co_await UserModel::createUser(username.get_string().c_str(),password.get_string().c_str(),email.get_string().c_str());

    response.result(boost::beast::http::status::ok);
    response.set(boost::beast::http::field::content_type,"application/json");
    boost::json::value res = boost::json::object();
    res.as_object()["status"] = "success";
    res.as_object()["message"] = "User Created Successfully!";
    response.body() = boost::json::serialize(res);
    co_return;
}

boost::asio::awaitable<void> UserHandler::loginUser(HttpRequest &&request, HttpResponse &response,const RequestParams &requestParams){
    
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
    
    if(!reqBody.get_object().contains("usernameEmail") || !reqBody.get_object().contains("password")){
        response.result(boost::beast::http::status::unprocessable_entity);
        response.set(boost::beast::http::field::content_type,"application/json");
    
        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "error";
        res.as_object()["message"] = "Required Fields not present in body!";
        
        response.body() = boost::json::serialize(res);
        co_return;
    }

    const boost::json::value usernameEmail = reqBody.get_object().at("usernameEmail");
    const boost::json::value password = reqBody.get_object().at("password"); 
    if(!usernameEmail.is_string() || !password.is_string()){
        response.result(boost::beast::http::status::unprocessable_entity);
        response.set(boost::beast::http::field::content_type,"application/json");
    
        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "error";
        res.as_object()["message"] = "Invalid Fields Type!";
        
        response.body() = boost::json::serialize(res);
        co_return;
    }
    if(usernameEmail.get_string().empty() || password.get_string().empty()){
        response.result(boost::beast::http::status::unprocessable_entity);
        response.set(boost::beast::http::field::content_type,"application/json");
    
        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "error";
        res.as_object()["message"] = "Required Fields cannot be empty!";
        
        response.body() = boost::json::serialize(res);
        co_return;
    }
    
    bool success = co_await UserModel::authenticateUser(usernameEmail.get_string().c_str(),password.get_string().c_str(),usernameEmail.get_string().c_str());
    if(success) {
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

boost::asio::awaitable<void> UserHandler::getUserByUUID(HttpRequest &&request, HttpResponse &response,const RequestParams &requestParams){

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
        response.body() = boost::json::serialize(res);
    }
    
    co_return;
}
