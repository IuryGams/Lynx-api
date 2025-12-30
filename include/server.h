#pragma once

#include "handlers/interface.h"

namespace lynx::server
{

struct ServerConfig
{
    int port = 8080;
    int threads = 2;
    std::string log_level = "info";
    bool cors = true;
    std::string cors_origin = "*";
};

class Server
{
private:
    ServerConfig config_;
    std::unique_ptr<App> app_;
    std::vector<std::shared_ptr<interface::IHandler>> handlers_;

    auto setup() -> void;

public:
    auto add_handler(std::shared_ptr<interface::IHandler> handler) -> void;
    explicit Server(const ServerConfig &config = ServerConfig());

    auto start() -> void;
};
} // namespace lynx::server
