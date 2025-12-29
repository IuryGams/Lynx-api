#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <vector>

namespace lynx::models::dto
{
struct OrderItemDTO
{
    int product_id;
    int quantity;
};

struct OrderCreateDTO
{
    int customer_id;
    std::vector<OrderItemDTO> items;
};

struct OrderResponseDTO
{
    int id;
    int customer_id;
    std::string status;
    std::chrono::system_clock::time_point created_at;
    std::vector<OrderItemDTO> items;

    // Opcional: incluir dados do cliente
    std::optional<std::string> customer_name;
    std::optional<std::string> customer_email;
};

struct OrderItemDetailsDTO
{
    int product_id;
    std::string product_name;
    int quantity;
    int unit_price_cents;
    int subtotal_cents;
};

struct OrderDetailsResponseDTO
{
    int order_id;
    int customer_id;
    std::string status;
    std::chrono::system_clock::time_point created_at;
    std::vector<OrderItemDetailsDTO> items;
    int64_t total_cents;
};

struct OrderSummaryDTO
{
    int id;
    int customer_id;
    std::string status;
    std::chrono::system_clock::time_point created_at;
    int64_t total_cents;
    int64_t total_paid_cents;
};
} // namespace lynx::models::dto
