#pragma once

#include "handlers/interface.h"
#include "services/order_services.h"


namespace lynx::controller
{

class OrderController : public interface::IHandler
{

private:
    std::shared_ptr<services::OrderServices> services_;
    std::string base_path_ = "/api/orders";

    auto create(const crow::request &req) -> crow::response;
    auto get(const int id) -> crow::response;             // handle_get_order
    auto list(const crow::request &req) -> crow::response; // handle_summary_orders
    auto update(const crow::request &req, int id) -> crow::response;
    auto remove(const crow::request &req) -> crow::response;

public:
    explicit OrderController(std::shared_ptr<services::OrderServices> services);

    auto register_routes(App &app) -> void override;
};

} // namespace lynx::controller
