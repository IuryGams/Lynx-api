#pragma once

#include "handlers/interface.h"
#include "services/payment_services.h"


namespace lynx::controller
{

class PaymentController : public interface::IHandler
{

private:
    std::shared_ptr<services::PaymentServices> services_;
    std::string base_path_ = "/api/payments";


    auto create(const crow::request &req) -> crow::response; // handle_create_payment
    auto get(const int &id) -> crow::response;               // handle_find_payment_by_id
    auto list(const crow::request &req) -> crow::response;   // handle_find_all_payments
    auto update(const crow::request &req, int &id) -> crow::response; // handle_update_payment
    auto remove(const crow::request &req) -> crow::response; // handle_delete_payment

public:
    explicit PaymentController(std::shared_ptr<services::PaymentServices> services);

    auto register_routes(App &app) -> void override;
};

} // namespace lynx::controller
