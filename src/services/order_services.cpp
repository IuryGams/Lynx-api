#include "services/order_services.h"
#include "errors/http_handle_error.h"
#include "utils/convert.h"
#include "utils/time/time_utils.h"
#include <stdexcept>

namespace lynx::services
{

OrderServices::OrderServices(std::shared_ptr<repository::interface::IOrderRepository> repository_order,
                             std::shared_ptr<repository::interface::IProductRepository> repository_product,
                             std::shared_ptr<repository::interface::ICustomerRepository> customer_repository)
    : repository_(repository_order)
{
    product_service_ = std::make_shared<ProductServices>(repository_product);
    customer_service_ = std::make_shared<CustomerServices>(customer_repository);
}

auto OrderServices::to_response_dto(const models::Order &order) -> models::dto::OrderResponseDTO
{
    // Converter os items do pedido para DTOs
    std::vector<models::dto::OrderItemDTO> items_dto;
    for (const auto &item : order.items)
    {
        items_dto.push_back(models::dto::OrderItemDTO{item.product_id, item.quantity});
    }

    models::dto::OrderResponseDTO dto{order.id, order.customer_id, utils::order_status_to_string(order.status), order.created_at, items_dto};

    // Preencher dados do cliente, se disponível
    if (order.customer.has_value())
    {
        dto.customer_name = order.customer->name;
        dto.customer_email = order.customer->email;
    }

    return dto;
}

auto OrderServices::to_summary_dto(const models::OrderSummary &summary) -> models::dto::OrderSummaryDTO
{
    return models::dto::OrderSummaryDTO{summary.id,         summary.customer_id, utils::order_status_to_string(summary.status),
                                        summary.created_at, summary.total_cents, summary.total_paid_cents};
}

auto OrderServices::validate_order(const models::Order &order) -> void
{
    if (order.items.empty())
    {
        throw exceptions::BadRequestError("Order must have at least one item");
    }

    for (const auto &item : order.items)
    {
        if (item.quantity <= 0)
            throw exceptions::BadRequestError("Item quantity must be greater than zero");
    }
}

auto OrderServices::validate_products(std::vector<models::OrderItem> &items) -> void
{
    for (auto &item : items)
    {
        auto product = product_service_->get_product_by_id(item.product_id);

        if (!product.active)
        {
            throw exceptions::BadRequestError("Product is not active: " + product.name);
        }

        item.unit_price_cents = product.price_cents;
    }
}

auto OrderServices::validate_customer(int customer_id) -> void
{
    customer_service_->get_customer_by_id(customer_id);
}

auto OrderServices::create_order(const models::dto::OrderCreateDTO &dto) -> models::dto::OrderResponseDTO
{
    models::Order order;
    order.customer_id = dto.customer_id;
    order.status = models::OrderStatus::NEW;
    order.created_at = std::chrono::system_clock::now();

    for (const auto &item_dto : dto.items)
    {
        models::OrderItem item;
        item.product_id = item_dto.product_id;
        item.quantity = item_dto.quantity;

        order.items.push_back(item);
    }

    validate_order(order);
    validate_products(order.items);
    validate_customer(order.customer_id);

    repository_->create(order);

    return to_response_dto(order);
}

auto OrderServices::get_order_by_id(const int &id) -> models::Order
{
    auto order_opt = repository_->find_by_id(id);
    if (!order_opt.has_value())
    {
        throw exceptions::NotFoundError("Order not found for order id: " + std::to_string(id));
    }

    return *order_opt;
}

auto OrderServices::get_order_with_customer(const int &id) -> models::Order
{

    auto order_with_customer_opt = repository_->find_by_id_with_customer(id);
    if (!order_with_customer_opt.has_value())
    {
        throw exceptions::NotFoundError("Order not found");
    }

    return *order_with_customer_opt;
}

auto OrderServices::get_all_orders() -> std::vector<models::Order>
{
    auto orders = repository_->find_all();

    std::vector<models::Order> summary;

    for (const auto &o : orders)
    {
        summary.push_back({o.id, o.customer_id, o.status, o.created_at});
    }

    return summary;
}

auto OrderServices::calculate_total_cents(int order_id) -> int64_t
{
    return repository_->sum_items_total_by_order(order_id);
}

auto OrderServices::get_all_orders_summary(const std::optional<std::string> &status_filter, const std::optional<int> &customer_id_filter,
                                           const std::optional<int> &limit) -> std::vector<models::dto::OrderSummaryDTO>
{
    auto summaries = repository_->find_all_summary(status_filter, customer_id_filter, limit);

    std::vector<models::dto::OrderSummaryDTO> dtos;
    dtos.reserve(summaries.size());

    for (const auto &summary : summaries)
    {
        dtos.push_back(to_summary_dto(summary));
    }

    return dtos;
}

auto OrderServices::mark_order_as_paid(const int &order_id) -> void
{
    auto order = get_order_by_id(order_id);

    if (order.status == models::OrderStatus::CANCELLED)
        throw exceptions::BadRequestError("Cannot pay a cancelled order");

    if (order.status == models::OrderStatus::PAID)
    {
        return; // idempotente
    }

    repository_->update_status(order.id, models::OrderStatus::PAID);
}

auto OrderServices::get_order_details(int order_id) -> models::dto::OrderDetailsResponseDTO
{
    auto order = get_order_by_id(order_id);

    auto items = repository_->find_items_by_order_id(order_id);
    if (items.empty())
    {
        throw exceptions::BadRequestError("Order has no items");
    }

    models::dto::OrderDetailsResponseDTO response;
    response.order_id = order.id;
    response.customer_id = order.customer_id;
    response.status = utils::order_status_to_string(order.status);
    response.created_at = order.created_at;
    response.total_cents = 0;

    for (const auto &item : items)
    {
        auto product = product_service_->get_product_by_id(item.product_id);

        models::dto::OrderItemDetailsDTO item_dto;
        item_dto.product_id = item.product_id;
        item_dto.product_name = product.name;
        item_dto.quantity = item.quantity;
        item_dto.unit_price_cents = item.unit_price_cents;
        item_dto.subtotal_cents = item.quantity * item.unit_price_cents;

        response.total_cents += item_dto.subtotal_cents;
        response.items.push_back(item_dto);
    }

    return response;
}

} // namespace lynx::services