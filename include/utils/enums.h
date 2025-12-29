#pragma once

enum class HttpStatus : int
{
    // 2XX
    OK = 200,
    CREATED = 201,

    // 4XX
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    NOT_FOUND = 404,
    FORBIDDEN = 403,
    CONFLICT = 409,

    // 5XX
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    BAD_GATEWAY = 502,
    SERVICE_UNAVAILABLE = 503,
};