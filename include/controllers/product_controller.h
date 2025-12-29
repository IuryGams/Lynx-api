#pragma once

#include "handlers/interface.h"
#include "services/product_services.h"


namespace lynx::controller
{

class ProductController : public interface::IHandler
{

private:
    std::shared_ptr<services::ProductServices> services_;
    std::string base_path_ = "/api/products";

    auto create(const crow::request &req) -> crow::response; // handle_create
    auto get(const int &id) -> crow::response;               // handle_find_by_id
    auto list(const crow::request &req) -> crow::response;   // handle_find_all
    auto update(const crow::request &req, int &id) -> crow::response; // handle_update
    auto remove(const crow::request &req) -> crow::response; // handle_delete

public:
    explicit ProductController(std::shared_ptr<services::ProductServices> services);

    auto register_routes(App &app) -> void override;
};

} // namespace lynx::controller
