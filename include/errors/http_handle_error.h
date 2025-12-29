#pragma once
#include "errors/handle_error.h"

namespace lynx::exceptions
{

// 4xx - Client Errors
class BadRequestError : public CustomError
{
public:
    BadRequestError(const std::string &message = "Bad Request")
        : CustomError("Bad Request", message, HttpStatus::BAD_REQUEST)
    {
    }
};

class UnauthorizedError : public CustomError
{
public:
    UnauthorizedError(const std::string &message = "Unauthorized")
        : CustomError("Unauthorized", message, HttpStatus::UNAUTHORIZED)
    {
    }
};

class ForbiddenError : public CustomError
{
public:
    ForbiddenError(const std::string &message = "Forbidden")
        : CustomError("Forbidden", message, HttpStatus::FORBIDDEN)
    {
    }
};

class NotFoundError : public CustomError
{
public:
    NotFoundError(const std::string &message = "Not Found")
        : CustomError("Not Found", message, HttpStatus::NOT_FOUND)
    {
    }
};

class ConflictError : public CustomError
{
public:
    ConflictError(const std::string &message = "Conflict")
        : CustomError("Conflict", message, HttpStatus::CONFLICT)
    {
    }
};

// 5xx - Server Errors
class InternalServerError : public CustomError
{
public:
    InternalServerError(const std::string &message = "Internal Server Error")
        : CustomError("Internal Server Error", message, HttpStatus::INTERNAL_SERVER_ERROR)
    {
    }
};

class NotImplementedError : public CustomError
{
public:
    NotImplementedError(const std::string &message = "Not Implemented")
        : CustomError("Not Implemented", message, HttpStatus::NOT_IMPLEMENTED)
    {
    }
};

class BadGatewayError : public CustomError
{
public:
    BadGatewayError(const std::string &message = "Bad Gateway")
        : CustomError("Bad Gateway", message, HttpStatus::BAD_GATEWAY)
    {
    }
};

class ServiceUnavailableError : public CustomError
{
public:
    ServiceUnavailableError(const std::string &message = "Service Unavailable")
        : CustomError("Service Unavailable", message, HttpStatus::SERVICE_UNAVAILABLE)
    {
    }
};

} // namespace lynx::exceptions
