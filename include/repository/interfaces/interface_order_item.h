#include "models/order_item.h"

namespace lynx::repository::interface
{

class IOrderItemRepository
{

public:
    virtual ~IOrderItemRepository() = default;

    virtual auto create(const models::OrderItem &item) -> void = 0;
    virtual auto find_by_order_id(int &order_id) -> std::vector<models::OrderItem> = 0;
};
} // namespace lynx::repository::interface