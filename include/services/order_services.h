#pragma once

#include "models/dtos/dto_orders.h"
#include "repository/interfaces/interface_order.h"
#include "services/customer_services.h"
#include "services/product_services.h"
#include <memory>
#include <string>

namespace lynx::services
{

class OrderServices
{
private:
    std::shared_ptr<repository::interface::IOrderRepository> repository_;
    std::shared_ptr<ProductServices> product_service_;
    std::shared_ptr<CustomerServices> customer_service_;

    auto validate_order(const models::Order &order) -> void;
    auto validate_products(std::vector<models::OrderItem> &items) -> void;
    auto validate_customer(int customer_id) -> void;

    auto to_response_dto(const models::Order &order) -> models::dto::OrderResponseDTO;
    auto to_summary_dto(const models::OrderSummary &summary) -> models::dto::OrderSummaryDTO;

public:
    OrderServices(std::shared_ptr<repository::interface::IOrderRepository> order_repository,
                  std::shared_ptr<repository::interface::IProductRepository> product_repository,
                  std::shared_ptr<repository::interface::ICustomerRepository> customer_repository);

    /* Criação de pedido */
    auto create_order(const models::dto::OrderCreateDTO &dto) -> models::dto::OrderResponseDTO;

    /* Consultas */
    auto get_order_by_id(const int &id) -> models::Order;
    auto get_order_with_customer(const int &id) -> models::Order;
    auto get_all_orders() -> std::vector<models::Order>;
    auto get_all_orders_summary(const std::optional<std::string>& status_filter,
    const std::optional<int>& customer_id_filter,
    const std::optional<int>& limit) -> std::vector<models::dto::OrderSummaryDTO>;
    auto get_order_details(int order_id) -> models::dto::OrderDetailsResponseDTO;

    /* Operações */
    auto mark_order_as_paid(const int &order_id) -> void;
    auto calculate_total_cents(int order_id) -> int64_t;
};

} // namespace lynx::services