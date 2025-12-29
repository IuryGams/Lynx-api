#include "controllers/product_controller.h"
#include "utils/convert.h"
#include "utils/enums.h"
#include "utils/time/time_utils.h"
#include <iostream>

namespace lynx::controller
{

ProductController::ProductController(std::shared_ptr<services::ProductServices> services)
    : services_(services)
{
}

auto ProductController::register_routes(App &app) -> void
{
    app.route_dynamic(this->base_path_).methods(crow::HTTPMethod::POST)([this](const crow::request &req) { return this->create(req); });
    app.route_dynamic(this->base_path_ + "/<int>").methods(crow::HTTPMethod::GET)([this](const crow::request &req, int id) { return this->get(id); });
    app.route_dynamic(this->base_path_).methods(crow::HTTPMethod::GET)([this](const crow::request &req) { return this->list(req); });
}

auto ProductController::create(const crow::request &req) -> crow::response
{
    auto body = crow::json::load(req.body);
    if (!body)
    {
        return crow::response((int)HttpStatus::BAD_REQUEST, "Invalid Json");
    }

    try
    {
        models::dto::ProductCreateDTO dto;
        dto.name = body["name"].s();
        dto.category = utils::string_to_category(body["category"].s());
        dto.price_cents = body["price_cents"].i();
        dto.active = body.has("active") ? body["active"].b() : true;

        auto response_dto = services_->create_product(dto);

        crow::json::wvalue res;
        res["id"] = response_dto.id;
        res["name"] = response_dto.name;
        res["category"] = utils::category_to_string(response_dto.category);
        res["price_cents"] = response_dto.price_cents;
        res["active"] = response_dto.active;

        return crow::response(static_cast<int>(HttpStatus::CREATED), res);
    }
    catch (const exceptions::CustomError &e)
    {
        return crow::response(static_cast<int>(e.status_code()), e.to_json());
    }
}

auto ProductController::get(const int &id) -> crow::response
{
    try
    {
        auto found_product = services_->get_product_by_id(id);

        crow::json::wvalue res;
        res["id"] = found_product.id;
        res["name"] = found_product.name;
        res["category"] = utils::category_to_string(found_product.category);
        res["price_cents"] = found_product.price_cents;
        res["active"] = found_product.active;

        return crow::response(static_cast<int>(HttpStatus::OK), res);
    }
    catch (const exceptions::CustomError &e)
    {
        return crow::response(static_cast<int>(e.status_code()), e.to_json());
    }
}

auto ProductController::list(const crow::request &req) -> crow::response
{
    try
    {
        models::ProductFilters filters;

        if (req.url_params.get("category"))
            filters.category = utils::string_to_category(req.url_params.get("category"));

        if (req.url_params.get("active"))
            filters.active = utils::string_to_bool(req.url_params.get("active"));

        if (req.url_params.get("min_price"))
            filters.min_price_cents = utils::string_to_int_or_throw(req.url_params.get("min_price"));

        if (req.url_params.get("max_price"))
            filters.max_price_cents = utils::string_to_int_or_throw(req.url_params.get("max_price"));

        auto products = services_->get_all_products(filters);

        crow::json::wvalue res;
        for (size_t i = 0; i < products.size(); ++i)
        {
            res[i]["id"] = products[i].id;
            res[i]["name"] = products[i].name;
            res[i]["category"] = utils::category_to_string(products[i].category);
            res[i]["price_cents"] = products[i].price_cents;
            res[i]["active"] = products[i].active;
        }

        return crow::response(static_cast<int>(HttpStatus::OK), res);
    }
    catch (const exceptions::CustomError &e)
    {
        return crow::response(static_cast<int>(e.status_code()), e.to_json());
    }
}

auto ProductController::update(const crow::request &req, int &id) -> crow::response
{
    auto body = crow::json::load(req.body);
    if (!body)
    {
        throw exceptions::BadRequestError("Invalid or missing JSON body");
    }
    try
    {
        models::dto::ProductCreateDTO dto;

        if (body.has("name"))
            dto.name = body["name"].s();

        if (body.has("category"))
            dto.category = utils::string_to_category(body["category"].s());

        if (body.has("price_cents"))
            dto.price_cents = body["price_cents"].i();

        if (body.has("active"))
            dto.active = body["active"].b();

        auto response_dto = services_->update_product(id, dto);

        crow::json::wvalue res;
        res["id"] = response_dto.id;
        res["name"] = response_dto.name;
        res["category"] = utils::category_to_string(response_dto.category);
        res["price_cents"] = response_dto.price_cents;
        res["active"] = response_dto.active;

        return crow::response(static_cast<int>(HttpStatus::OK), res);
    }
    catch (const exceptions::CustomError &e)
    {
        return crow::response(static_cast<int>(e.status_code()), e.to_json());
    }
}

auto ProductController::remove(const crow::request &req) -> crow::response
{
    try
    {
        return crow::response((int)HttpStatus::OK, "Product created successfully");
    }
    catch (const exceptions::CustomError &e)
    {
        return crow::response(static_cast<int>(e.status_code()), e.to_json());
    }
}

} // namespace lynx::controller