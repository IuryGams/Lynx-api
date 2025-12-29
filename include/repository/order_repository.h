#pragma once

#include "repository/database/sqlite/sqlite_base_repository.h"
#include "repository/interfaces/interface_order.h"
#include <cstdint>

namespace lynx::repository
{

class OrderRepository final : public interface::IOrderRepository, protected SQLiteBaseRepository
{
public:
    OrderRepository();

    auto create(models::Order &order) -> void override;
    auto find_by_id(int id) -> std::optional<models::Order> override;
    auto find_by_id_with_customer(int id) -> std::optional<models::Order> override;
    auto find_all() -> std::vector<models::Order> override;
    
    auto update(const int &id, const models::Order &order) -> void override;
    auto update_status(int order_id, const models::OrderStatus &status) -> void override;
    auto remove(int id) -> void override;
    
    /* Order Items */
    auto find_items_by_order_id(int order_id) -> std::vector<models::OrderItem> override;
    auto sum_items_total_by_order(int order_id) -> int64_t override;

    /* Summary */
    auto find_all_summary(const std::optional<std::string> &status_filter, const std::optional<int> &customer_id_filter,
                                       const std::optional<int> &limit) -> std::vector<models::OrderSummary> override;
};

} // namespace lynx::repository
