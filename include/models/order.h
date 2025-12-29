#pragma once

#include "customers.h"
#include "order_item.h"
#include <chrono>
#include <utility>
#include <vector>

namespace lynx::models
{

enum class OrderStatus
{
    NEW,
    PAID,
    CANCELLED
};

struct Order
{
    int id;
    int customer_id;
    OrderStatus status;
    std::chrono::system_clock::time_point created_at;
    std::vector<OrderItem> items;
    std::optional<Customer> customer;
};

struct OrderSummary
{
    int id;
    int customer_id;
    models::OrderStatus status;
    std::chrono::system_clock::time_point created_at;
    int64_t total_cents;
    int64_t total_paid_cents;
};

struct OrderItemDetails
{
    int product_id;
    std::string product_name;
    int quantity;
    int unit_price_cents;
    int subtotal_cents;
};

struct OrderDetailsResponse
{
    int order_id;
    int customer_id;
    OrderStatus status;
    std::chrono::system_clock::time_point created_at;
    std::vector<OrderItemDetails> items;
    int total_cents;
};

} // namespace lynx::models