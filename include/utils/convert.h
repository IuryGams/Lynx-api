#pragma once
#include "errors/http_handle_error.h"
#include "models/order.h"
#include "models/payment.h"
#include "models/product.h"
#include <optional>
#include <string>

namespace utils
{

inline auto string_to_category(const std::string &text) -> lynx::models::Category
{
    if (text == "KITCHEN")
        return lynx::models::Category::KITCHEN;
    if (text == "ELECTRONICS")
        return lynx::models::Category::ELECTRONICS;
    if (text == "HOME")
        return lynx::models::Category::HOME;
    if (text == "CLEANING")
        return lynx::models::Category::CLEANING;
    if (text == "FOOD")
        return lynx::models::Category::FOOD;
    if (text == "BEVERAGES")
        return lynx::models::Category::BEVERAGES;
    if (text == "PERSONAL_CARE")
        return lynx::models::Category::PERSONAL_CARE;
    if (text == "PETS")
        return lynx::models::Category::PETS;
    if (text == "TOOLS")
        return lynx::models::Category::TOOLS;
    if (text == "OFFICE")
        return lynx::models::Category::OFFICE;
    if (text == "TOYS")
        return lynx::models::Category::TOYS;
    if (text == "CLOTHING")
        return lynx::models::Category::CLOTHING;
    if (text == "OTHER")
        return lynx::models::Category::OTHER;

    throw lynx::exceptions::BadRequestError("Invalid product category: " + text);
};

inline auto category_to_string(const lynx::models::Category &category) -> std::string
{
    switch (category)
    {
    case lynx::models::Category::KITCHEN:
        return "KITCHEN";
    case lynx::models::Category::ELECTRONICS:
        return "ELECTRONICS";
    case lynx::models::Category::HOME:
        return "HOME";
    case lynx::models::Category::CLEANING:
        return "CLEANING";
    case lynx::models::Category::FOOD:
        return "FOOD";
    case lynx::models::Category::BEVERAGES:
        return "BEVERAGES";
    case lynx::models::Category::PERSONAL_CARE:
        return "PERSONAL_CARE";
    case lynx::models::Category::PETS:
        return "PETS";
    case lynx::models::Category::TOOLS:
        return "TOOLS";
    case lynx::models::Category::OFFICE:
        return "OFFICE";
    case lynx::models::Category::TOYS:
        return "TOYS";
    case lynx::models::Category::CLOTHING:
        return "CLOTHING";
    case lynx::models::Category::OTHER:
        return "OTHER";
    }

    throw lynx::exceptions::BadRequestError("Invalid product category");
};

inline auto string_to_bool(const std::string &s) -> std::optional<bool>
{
    if (s == "1" || s == "true")
        return true;
    if (s == "0" || s == "false")
        return false;

    return std::nullopt;
}

inline int string_to_int_or_throw(const std::string &s)
{
    try
    {
        size_t pos;
        int value = std::stoi(s, &pos);
        if (pos != s.size())
            throw lynx::exceptions::BadRequestError("Invalid integer: " + s);
        return value;
    }
    catch (const std::invalid_argument &)
    {
        throw lynx::exceptions::BadRequestError("Invalid integer: " + s);
    }
    catch (const std::out_of_range &)
    {
        throw lynx::exceptions::BadRequestError("Integer out of range: " + s);
    }
}

inline auto order_status_to_string(lynx::models::OrderStatus status) -> std::string
{
    switch (status)
    {
    case lynx::models::OrderStatus::NEW:
        return "NEW";
    case lynx::models::OrderStatus::PAID:
        return "PAID";
    case lynx::models::OrderStatus::CANCELLED:
        return "CANCELLED";
    }
    throw lynx::exceptions::InternalServerError("Invalid order status enum value");
}

inline auto string_to_order_status(const std::string &str) -> lynx::models::OrderStatus
{
    if (str == "NEW")
        return lynx::models::OrderStatus::NEW;
    if (str == "PAID")
        return lynx::models::OrderStatus::PAID;
    if (str == "CANCELLED")
        return lynx::models::OrderStatus::CANCELLED;
    throw lynx::exceptions::BadRequestError("Invalid order status: " + str);
}

inline auto string_to_payment_method(const std::string &str) -> lynx::models::PaymentMethod
{
    if (str == "PIX")
        return lynx::models::PaymentMethod::PIX;
    if (str == "CARD")
        return lynx::models::PaymentMethod::CARD;
    if (str == "BOLETO")
        return lynx::models::PaymentMethod::BOLETO;
    throw lynx::exceptions::BadRequestError("Invalid order status: " + str);
}

inline auto payment_method_to_string(lynx::models::PaymentMethod status) -> std::string
{
    switch (status)
    {
    case lynx::models::PaymentMethod::PIX:
        return "PIX";
    case lynx::models::PaymentMethod::CARD:
        return "CARD";
    case lynx::models::PaymentMethod::BOLETO:
        return "BOLETO";
    }
    throw lynx::exceptions::InternalServerError("Invalid order status enum value");
}
} // namespace utils