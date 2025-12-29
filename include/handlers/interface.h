#pragma once

#include <crow.h>
#include <crow/middlewares/cors.h>
#include <string>

using App = crow::App<crow::CORSHandler>;

namespace lynx::interface
{
class IHandler
{
public:
    virtual void register_routes(App &app) = 0;

};
} // namespace lynx::interface
