#include "controllers/customer_controller.h"
#include "errors/handle_error.h"
#include "middleware/validate_body.h"
#include "utils/enums.h"
#include "utils/time/time_utils.h"
#include <iostream>

namespace lynx::controller
{

CustomerController::CustomerController(std::shared_ptr<services::CustomerServices> services)
    : services_(services)
{
}

auto CustomerController::register_routes(App &app) -> void
{
    app.route_dynamic(this->base_path_).methods(crow::HTTPMethod::POST)([this](const crow::request &req) { return this->create(req); });

    app.route_dynamic(this->base_path_ + "/<int>").methods(crow::HTTPMethod::GET)([this](const crow::request &req, int id) { return this->get(id); });
}

auto CustomerController::create(const crow::request &req) -> crow::response
{
    try
    {
        auto body = crow::json::load(req.body);
        if (!body.has("name") || !body.has("email"))
        {
            throw exceptions::BadRequestError("Missing required fields");
        }

        models::dto::CustomerCreateDTO dto;
        dto.name = body["name"].s();
        dto.email = body["email"].s();

        auto result = services_->create_customer(dto);

        crow::json::wvalue res;
        res["id"] = result.id;
        res["name"] = result.name;
        res["email"] = result.email;
        res["created_at"] = utils::time::time_point_to_string(result.created_at);

        return crow::response((int)HttpStatus::CREATED, res);
    }
    catch (const exceptions::CustomError &e)
    {
        return crow::response(static_cast<int>(e.status_code()), e.to_json());
    }
}

auto CustomerController::get(const int &id) -> crow::response
{
    try
    {
        auto result = services_->get_customer_by_id(id);

        crow::json::wvalue res;
        res["id"] = result.id;
        res["name"] = result.name;
        res["email"] = result.email;
        res["created_at"] = utils::time::time_point_to_string(result.created_at);

        return crow::response((int)HttpStatus::OK, res);
    }
    catch (const exceptions::CustomError &e)
    {
        return crow::response(static_cast<int>(e.status_code()), e.to_json());
    }
}

auto CustomerController::list(const crow::request &req) -> crow::response
{
    try
    {
        auto customers = services_->get_all_customers();

        crow::json::wvalue res;

        for (size_t i = 0; i < customers.size(); i++)
        {
            res[i]["id"] = customers[i].id;
            res[i]["name"] = customers[i].name;
            res[i]["email"] = customers[i].email;
            res[i]["created_at"] = utils::time::time_point_to_string(customers[i].created_at);
        }

        return crow::response((int)HttpStatus::OK, res);
    }
    catch (const exceptions::CustomError &e)
    {
        return crow::response(static_cast<int>(e.status_code()), e.to_json());
    }
}

auto CustomerController::update(const crow::request &req, int &id) -> crow::response
{
    try
    {
        return crow::response((int)HttpStatus::OK, "Customer created successfully");
    }
    catch (const exceptions::CustomError &e)
    {
        return crow::response(static_cast<int>(e.status_code()), e.to_json());
    }
}

auto CustomerController::remove(const crow::request &req) -> crow::response
{
    try
    {
        // 2. Chama o Service
        /* services_->create_customer(name, email); */

        // 3. Retorna sucesso
        return crow::response((int)HttpStatus::OK, "Customer created successfully");
    }
    catch (const exceptions::CustomError &e)
    {
        return crow::response(static_cast<int>(e.status_code()), e.to_json());
    }
}

} // namespace lynx::controller