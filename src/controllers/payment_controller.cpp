#include "controllers/payment_controller.h"
#include "errors/handle_error.h"
#include "middleware/validate_body.h"
#include "utils/convert.h"
#include "utils/enums.h"
#include "utils/time/time_utils.h"

namespace lynx::controller
{

PaymentController::PaymentController(std::shared_ptr<services::PaymentServices> services)
    : services_(services)
{
}

auto PaymentController::register_routes(App &app) -> void
{
    app.route_dynamic(this->base_path_).methods(crow::HTTPMethod::POST)([this](const crow::request &req) { return this->create(req); });
    app.route_dynamic(this->base_path_ + "/<int>").methods(crow::HTTPMethod::GET)([this](const crow::request &req, int id) { return this->get(id); });
    app.route_dynamic(this->base_path_).methods(crow::HTTPMethod::GET)([this](const crow::request &req) { return this->list(req); });
}

auto PaymentController::create(const crow::request &req) -> crow::response
{
    try
    {
        auto body = crow::json::load(req.body);

        models::dto::PaymentCreateDTO dto;
        dto.method = utils::string_to_payment_method(body["method"].s());
        dto.amount_cents = body["amount_cents"].i();
        dto.order_id = body["order_id"].i();

        auto payment_res = services_->create_payment(dto);

        crow::json::wvalue res;
        res["id"] = payment_res.id;
        res["order_id"] = payment_res.order_id;
        res["method"] = utils::payment_method_to_string(payment_res.method);
        res["amount_cents"] = payment_res.amount_cents;
        res["paid_at"] = utils::time::time_point_to_string(payment_res.paid_at);

        return crow::response((int)HttpStatus::CREATED, res);
    }
    catch (const exceptions::CustomError &e)
    {
        return crow::response(static_cast<int>(e.status_code()), e.to_json());
    }
}

auto PaymentController::get(const int &payment_id) -> crow::response
{
    try
    {
        auto payment_res = services_->get_payment_by_id(payment_id);

        crow::json::wvalue res;
        res["id"] = payment_res.id;
        res["order_id"] = payment_res.order_id;
        res["method"] = utils::payment_method_to_string(payment_res.method);
        res["amount_cents"] = payment_res.amount_cents;
        res["paid_at"] = utils::time::time_point_to_string(payment_res.paid_at);

        return crow::response((int)(HttpStatus::OK), res);
    }
    catch (const exceptions::CustomError &e)
    {
        return crow::response(static_cast<int>(e.status_code()), e.to_json());
    }
}

auto PaymentController::list(const crow::request &req) -> crow::response
{
    return crow::response((int)HttpStatus::OK, "res");
}
auto PaymentController::update(const crow::request &req, int &id) -> crow::response
{
    return crow::response((int)HttpStatus::OK, "res");
}
auto PaymentController::remove(const crow::request &req) -> crow::response
{
    return crow::response((int)HttpStatus::OK, "res");
}

} // namespace lynx::controller