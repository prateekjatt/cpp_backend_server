#ifndef _EMAIL_SERVICE_H_
#define _EMAIL_SERVICE_H_

#include <string>
#include <boost/asio.hpp>

namespace EmailService {
    boost::asio::awaitable<int> sendEmail(const std::string toEmail, const std::string subject, const std::string body);
}

#endif