#pragma once
#include "repository/database/sqlite/sqlite_base_repository.h"
#include "repository/interfaces/interface_order_item.h"

namespace lynx::repository
{

class OrderItemRepository final : public interface::IOrderItemRepository, protected SQLiteBaseRepository
{
    public: 
    explicit OrderItemRepository();

    auto create(const models::OrderItem &item) -> void override;
    auto find_by_order_id(int &order_id) -> std::vector<models::OrderItem> override;
};

} // namespace lynx::repository
