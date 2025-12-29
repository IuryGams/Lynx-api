#pragma once

#include "services/customer_services.h"

#include "handlers/interface.h"

namespace lynx::controller
{

class CustomerController : public interface::IHandler
{

private:
    std::shared_ptr<services::CustomerServices> services_;
    std::string base_path_ = "/api/customers";

    auto create(const crow::request &req) -> crow::response;
    auto get(const int &id) -> crow::response;
    auto list(const crow::request &req) -> crow::response;
    auto update(const crow::request &req, int &id) -> crow::response;
    auto remove(const crow::request &req) -> crow::response;

public:
    explicit CustomerController(std::shared_ptr<services::CustomerServices> services);

    auto register_routes(App &app) -> void override;
};

} // namespace lynx::controller
