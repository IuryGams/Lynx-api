#pragma once

#include "models/dtos/dto_payment.h"
#include "repository/interfaces/interface_payment.h"
#include "services/order_services.h"
#include <memory>


namespace lynx::services
{

class PaymentServices
{
private:
    std::shared_ptr<repository::interface::IPaymentRepository> repository_;
    std::shared_ptr<OrderServices> order_service_; // Para validar pedidos e total

    auto validate_payment(const models::Payment &payment) -> void;

    auto to_response_dto(const models::Payment &payment) -> models::dto::PaymentResponseDTO;

public:
    PaymentServices(std::shared_ptr<repository::interface::IPaymentRepository> payment_repository, std::shared_ptr<OrderServices> order_service);

    auto create_payment(const models::dto::PaymentCreateDTO &dto) -> models::dto::PaymentResponseDTO;
    auto get_payment_by_id(int payment_id) -> models::dto::PaymentResponseDTO;
};
} // namespace lynx::services