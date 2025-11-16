#include "email_service.h"
#include <boost/asio/ssl.hpp>
#include "../utils/globals.h"
#include "../utils/configuration_manager.h"
#include "../utils/logger.h"

boost::asio::awaitable<int> EmailService::sendEmail(const std::string toEmail, const std::string subject, const std::string body){
    const std::string smtpServerName = ConfigurationManager::get("SMTP_SERVER_HOSTNAME");
    const std::string smtpServerPort = ConfigurationManager::get("SMTP_SERVER_PORT");

    boost::asio::ip::tcp::resolver resolver(Globals::ioContext);

    boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23_client);
    ctx.use_certificate_file(ConfigurationManager::get("SSL_CERTIFICATE_FILE"),boost::asio::ssl::context_base::file_format::pem);
    ctx.use_private_key_file(ConfigurationManager::get("SSL_PRIVATE_KEY_FILE"),boost::asio::ssl::context::file_format::pem);
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_stream(Globals::ioContext,ctx);
    auto endpoints = resolver.resolve(smtpServerName,smtpServerPort);

    co_await boost::asio::async_connect(ssl_stream.lowest_layer(),endpoints,boost::asio::use_awaitable);
    co_await ssl_stream.async_handshake(boost::asio::ssl::stream_base::handshake_type::client,boost::asio::use_awaitable);
    std::string msg = 
    "HELO example.com\r\n"
    "AUTH LOGIN\r\n"
    +ConfigurationManager::get("SMTP_USERNAME_BASE64")+"\r\n"
    +ConfigurationManager::get("SMTP_PASSWORD_BASE64")+"\r\n"
    "MAIL FROM: <support@example.com>\r\n"
    "RCPT TO: <"+toEmail+">\r\n"
    "DATA\r\n"
    "From: Example <support@example.com>\r\n"
    "To: <"+toEmail+">\r\n"
    "Subject:"+subject+"\r\n\r\n"
    +body+"\r\n.\r\n"
    "QUIT\r\n\r\n";
    
    co_await boost::asio::async_write(ssl_stream,boost::asio::buffer(msg),boost::asio::use_awaitable);
    
    boost::system::error_code ec;
    ssl_stream.shutdown(ec);

    co_return 0;
}