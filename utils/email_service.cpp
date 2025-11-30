#include "email_service.h"
#include <boost/asio/ssl.hpp>
#include "../utils/globals.h"
#include "../utils/configuration_manager.h"
#include "../utils/logger.h"
#include <sodium.h>

boost::asio::awaitable<int> EmailService::sendEmail(const std::string toEmail, const EmailType emailType, const std::unordered_map<std::string,std::string> &data){
    const std::string smtpServerName = ConfigurationManager::get("SMTP_SERVER_HOSTNAME");
    const std::string smtpServerPort = ConfigurationManager::get("SMTP_SERVER_PORT");

    size_t encodedUsernameSize = sodium_base64_ENCODED_LEN(ConfigurationManager::get("SMTP_USERNAME_BASE64").length(),sodium_base64_VARIANT_ORIGINAL); 
    size_t encodedPasswordSize = sodium_base64_ENCODED_LEN(ConfigurationManager::get("SMTP_PASSWORD_BASE64").length(),sodium_base64_VARIANT_ORIGINAL); 
    char* base64Username = new char[encodedUsernameSize];
    char* base64Password = new char[encodedPasswordSize];
    sodium_bin2base64(base64Username,encodedUsernameSize,(const unsigned char*)ConfigurationManager::get("SMTP_USERNAME_BASE64").c_str(),ConfigurationManager::get("SMTP_USERNAME_BASE64").length(),sodium_base64_VARIANT_ORIGINAL);
    sodium_bin2base64(base64Password,encodedPasswordSize,(const unsigned char*)ConfigurationManager::get("SMTP_PASSWORD_BASE64").c_str(),ConfigurationManager::get("SMTP_PASSWORD_BASE64").length(),sodium_base64_VARIANT_ORIGINAL);

    std::string subject = "";
    std::string body = "";

    if(emailType == WELCOME_EMAIL){
        subject = "Welcome!";
        body = "Hi "+data.at("username")+",\n\n"
        "Welcome! We're so glad you've joined us.\n"
        "We will keep you updated about the development.\n"
        +"\nTeam";
    } else if(emailType == SIGNUP_OTP_EMAIL){
        subject = "OTP for Signup";
        body = "Your OTP for creating new account is "
        +data.at("OTP")+
        "\nOTP will be valid for 10 minutes"
        "\n\nTeam";
    }
    
    std::string msg = 
    "HELO example.com\r\n"
    "AUTH LOGIN\r\n"
    +std::string(base64Username)+"\r\n"
    +std::string(base64Password)+"\r\n"
    "MAIL FROM: <support@example.com>\r\n"
    "RCPT TO: <"+toEmail+">\r\n"
    "DATA\r\n"
    "From: Example <support@example.com>\r\n"
    "To: <"+toEmail+">\r\n"
    "Subject:"+subject+"\r\n\r\n"
    +body+"\r\n.\r\n"
    "QUIT\r\n\r\n";
    
    boost::asio::ip::tcp::resolver resolver(Globals::ioContext);

    boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23_client);
    ctx.use_certificate_file(ConfigurationManager::get("SSL_CERTIFICATE_FILE"),boost::asio::ssl::context_base::file_format::pem);
    ctx.use_private_key_file(ConfigurationManager::get("SSL_PRIVATE_KEY_FILE"),boost::asio::ssl::context::file_format::pem);
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_stream(Globals::ioContext,ctx);
    auto endpoints = resolver.resolve(smtpServerName,smtpServerPort);

    co_await boost::asio::async_connect(ssl_stream.lowest_layer(),endpoints,boost::asio::use_awaitable);
    co_await ssl_stream.async_handshake(boost::asio::ssl::stream_base::handshake_type::client,boost::asio::use_awaitable);

    co_await boost::asio::async_write(ssl_stream,boost::asio::buffer(msg),boost::asio::use_awaitable);
    
    boost::system::error_code ec;
    ssl_stream.shutdown(ec);

    co_return 0;
}