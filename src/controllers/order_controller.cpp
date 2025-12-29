#include "controllers/order_controller.h"
#include "errors/handle_error.h"
#include "utils/convert.h"
#include "utils/enums.h"
#include "utils/time/time_utils.h"

namespace lynx::controller
{

OrderController::OrderController(std::shared_ptr<services::OrderServices> services)
    : services_(services)
{
}

auto OrderController::register_routes(App &app) -> void
{
    app.route_dynamic(this->base_path_).methods(crow::HTTPMethod::POST)([this](const crow::request &req) { return this->create(req); });
    app.route_dynamic(this->base_path_ + "/<int>").methods(crow::HTTPMethod::GET)([this](const crow::request &req, int id) { return this->get(id); });
    app.route_dynamic(this->base_path_ + "/summary").methods(crow::HTTPMethod::GET)([this](const crow::request &req) { return this->list(req); });
}

auto OrderController::create(const crow::request &req) -> crow::response
{
    try
    {
        auto body = crow::json::load(req.body);
        if (!body)
            return crow::response(400, "Invalid JSON");

        models::dto::OrderCreateDTO dto;
        if (!body.has("customer_id") || !body.has("items"))
        {
            throw exceptions::BadRequestError("Missing required fields");
        }

        dto.customer_id = body["customer_id"].i();

        for (const auto &jitem : body["items"])
        {
            models::dto::OrderItemDTO item_dto;
            item_dto.product_id = jitem["product_id"].i();
            item_dto.quantity = jitem["quantity"].i();
            dto.items.push_back(item_dto);
        }

        auto created_order = services_->create_order(dto);

        crow::json::wvalue res;
        res["message"] = "Order created successfully";
        res["order_id"] = created_order.id;

        return crow::response((int)HttpStatus::CREATED, res);
    }
    catch (const exceptions::CustomError &e)
    {
        return crow::response(static_cast<int>(e.status_code()), e.to_json());
    }
}

auto OrderController::get(const int order_id) -> crow::response
{
    try
    {
        auto order = services_->get_order_details(order_id);

        crow::json::wvalue res;
        res["order_id"] = order.order_id;
        res["customer_id"] = order.customer_id;
        res["status"] = order.status;
        res["created_at"] = utils::time::time_point_to_string(order.created_at);

        res["items"] = crow::json::wvalue::list();
        for (size_t i = 0; i < order.items.size(); ++i)
        {
            const auto &item = order.items[i];
            res["items"][i]["product_id"] = item.product_id;
            res["items"][i]["product_name"] = item.product_name;
            res["items"][i]["quantity"] = item.quantity;
            res["items"][i]["unit_price_cents"] = item.unit_price_cents;
            res["items"][i]["subtotal_cents"] = item.subtotal_cents;
        }

        res["total_cents"] = order.total_cents;

        return crow::response((int)(HttpStatus::OK), res);
    }
    catch (const lynx::exceptions::CustomError &e)
    {
        return crow::response((int)(e.status_code()), e.to_json());
    }
}

auto OrderController::list(const crow::request &req) -> crow::response
{
    try
    {
        // 1️⃣ Capturar query params opcionais
        std::optional<std::string> status_filter;
        if (auto status = req.url_params.get("status"))
            status_filter = std::string(status);

        std::optional<int> customer_id_filter;
        if (auto cid = req.url_params.get("customer_id"))
            customer_id_filter = std::stoi(cid);

        std::optional<int> limit;
        if (auto lim = req.url_params.get("limit"))
            limit = std::stoi(lim);

        // 2️⃣ Buscar dados do serviço com filtros
        auto summary_orders = services_->get_all_orders_summary(status_filter, customer_id_filter, limit);

        // 3️⃣ Montar JSON de resposta
        crow::json::wvalue res = crow::json::wvalue::list();
        for (size_t i = 0; i < summary_orders.size(); ++i)
        {
            const auto &order = summary_orders[i];
            res[i]["id"] = order.id;
            res[i]["customer_id"] = order.customer_id;
            res[i]["status"] = order.status;
            res[i]["created_at"] = utils::time::time_point_to_string(order.created_at);
            res[i]["total_cents"] = order.total_cents;
            res[i]["total_paid_cents"] = order.total_paid_cents;
        }

        return crow::response((int)HttpStatus::OK, res);
    }
    catch (const exceptions::CustomError &e)
    {
        return crow::response(static_cast<int>(e.status_code()), e.to_json());
    }
}

auto OrderController::update(const crow::request &req, int id) -> crow::response
{
    try
    {
        return crow::response((int)HttpStatus::OK, "Order created successfully");
    }
    catch (const exceptions::CustomError &e)
    {
        return crow::response(static_cast<int>(e.status_code()), e.to_json());
    }
}

auto OrderController::remove(const crow::request &req) -> crow::response
{
    try
    {
        return crow::response((int)HttpStatus::OK, "Order created successfully");
    }
    catch (const exceptions::CustomError &e)
    {
        return crow::response(static_cast<int>(e.status_code()), e.to_json());
    }
}

} // namespace lynx::controller