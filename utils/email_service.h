#ifndef _EMAIL_SERVICE_H_
#define _EMAIL_SERVICE_H_

#include <string>
#include <boost/asio.hpp>

namespace EmailService {
    enum EmailType{
        WELCOME_EMAIL,
        SIGNUP_OTP_EMAIL
    };
    boost::asio::awaitable<int> sendEmail(const std::string toEmail, const EmailType emailType, const std::unordered_map<std::string,std::string> &data);
}

#endif