#pragma once

#include "repository/interfaces/interface_order_item.h"
#include <memory>
#include <string>

namespace lynx::services
{

class OrderItemServices
{
private:
    std::shared_ptr<repository::interface::IOrderItemRepository> repository_;

    auto validate_order_item(const models::OrderItem &OrderItem) -> void;

public:
    OrderItemServices(std::shared_ptr<repository::interface::IOrderItemRepository> repository);

    auto create_items(const int &order_id, const std::vector<models::OrderItem> &items) -> void;
    auto get_items_by_order(int &order_id) -> std::vector<models::OrderItem>;
};

} // namespace lynx::services