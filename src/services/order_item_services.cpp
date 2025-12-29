#include "services/order_item_services.h"

namespace lynx::services
{

OrderItemServices::OrderItemServices(std::shared_ptr<repository::interface::IOrderItemRepository> repository)
    : repository_(repository) {

    };

auto OrderItemServices::create_items(const int &order_id, const std::vector<models::OrderItem> &items) -> void
{
    for (auto item : items)
    {
        item.order_id = order_id;
        repository_->create(item);
    }
}
auto OrderItemServices::get_items_by_order(int &order_id) -> std::vector<models::OrderItem>
{
    return repository_->find_by_order_id(order_id);
}

} // namespace lynx::services
