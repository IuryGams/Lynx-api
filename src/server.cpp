#include "server.h"
#include "controllers/customer_controller.h"

namespace lynx::server
{

Server::Server(const ServerConfig &config)
    : config_(config)
{
    if (this->config_.log_level == "debug")
    {
        crow::logger::setLogLevel(crow::LogLevel::Debug);
    }
    else if (this->config_.log_level == "info")
    {
        crow::logger::setLogLevel(crow::LogLevel::Info);
    }
    else if (this->config_.log_level == "warning")
    {
        crow::logger::setLogLevel(crow::LogLevel::Warning);
    }
    else if (this->config_.log_level == "error")
    {
        crow::logger::setLogLevel(crow::LogLevel::Error);
    }
    else if (this->config_.log_level == "critical")
    {
        crow::logger::setLogLevel(crow::LogLevel::Critical);
    }
    else
    {
        crow::logger::setLogLevel(crow::LogLevel::Info);
    }

    // App Init
    this->app_ = std::make_unique<App>();

    // Config cors
    if (this->config_.cors)
    {
        auto &cors = this->app_->get_middleware<crow::CORSHandler>();

        cors.global()
            .methods("GET"_method, "POST"_method, "PATCH"_method, "DELETE"_method, "OPTIONS"_method)
            .headers("Content-Type", "Authorization", "X-Custom-Header")
            .origin(this->config_.cors_origin)
            .prefix("/api")
            .max_age(86400);
    }
}

auto Server::setup() -> void
{

    for (auto &handler : handlers_)
    {
        handler->register_routes(*app_);
    }
}

auto Server::add_handler(std::shared_ptr<interface::IHandler> handler) -> void
{
    this->handlers_.push_back(std::move(handler));
}

auto Server::start() -> void
{
    this->setup();
    this->app_->port(this->config_.port).multithreaded().concurrency(this->config_.threads).run_async();
}
} // namespace lynx::server