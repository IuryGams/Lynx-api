#pragma once

#include "models/order.h"
#include <cstdint>
#include <optional>
#include <vector>

namespace lynx::repository::interface
{

class IOrderRepository
{
public:
    virtual ~IOrderRepository() = default;

    virtual auto create(models::Order &order) -> void = 0;
    virtual auto find_by_id(int id) -> std::optional<models::Order> = 0;
    virtual auto find_all() -> std::vector<models::Order> = 0;
    virtual auto find_by_id_with_customer(int id) -> std::optional<models::Order> = 0;
    virtual auto update(const int &id, const models::Order &order) -> void = 0;
    virtual auto update_status(int order_id, const models::OrderStatus &status) -> void = 0;
    virtual auto remove(int id) -> void = 0;

    // Order Items
    virtual auto find_items_by_order_id(int order_id) -> std::vector<models::OrderItem> = 0;
    virtual auto sum_items_total_by_order(int order_id) -> int64_t = 0;

    /* Summary */
    virtual auto find_all_summary(const std::optional<std::string> &status_filter, const std::optional<int> &customer_id_filter,
                                       const std::optional<int> &limit) -> std::vector<models::OrderSummary> = 0;
};
} // namespace lynx::repository::interface