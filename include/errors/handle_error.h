#pragma once
#include "utils/enums.h"
#include <exception>
#include <sstream>
#include <string>

namespace lynx::exceptions
{
class CustomError : public std::exception
{
private:
    std::string name_error_;
    HttpStatus status_code_;
    bool is_operational_;
    std::string message_;

public:
    explicit CustomError(const std::string &name_error = "Error", const std::string &message = "Internal Server Error",
                         HttpStatus status_code = HttpStatus::INTERNAL_SERVER_ERROR, bool is_operational = true)
        : name_error_(name_error)
        , status_code_(status_code)
        , is_operational_(is_operational)
        , message_(message)
    {
    }

    const char *what() const noexcept override
    {
        return message_.c_str();
    }

    const std::string &name() const
    {
        return name_error_;
    }
    HttpStatus status_code() const
    {
        return status_code_;
    }
    bool is_operational() const
    {
        return is_operational_;
    }

    std::string to_string() const
    {
        std::ostringstream oss;
        oss << "Error: " << name_error_ << ", Message: " << message_ << ", StatusCode: " << static_cast<int>(status_code_);
        return oss.str();
    }

    std::string to_json() const
    {
        std::ostringstream oss;
        oss << "{"
            << "\"error\":\"" << name_error_ << "\","
            << "\"message\":\"" << message_ << "\","
            << "\"statusCode\":" << static_cast<int>(status_code_) << "}";
        return oss.str();
    }
};
} // namespace lynx::exceptions
