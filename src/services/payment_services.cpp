#include "services/payment_services.h"
#include "errors/http_handle_error.h"
#include "utils/time/time_utils.h"

namespace lynx::services
{

PaymentServices::PaymentServices(std::shared_ptr<repository::interface::IPaymentRepository> payment_repository,
                                 std::shared_ptr<OrderServices> order_service)
    : repository_(payment_repository)
    , order_service_(order_service)
{
}

auto PaymentServices::to_response_dto(const models::Payment &payment) -> models::dto::PaymentResponseDTO
{
    return models::dto::PaymentResponseDTO{payment.id, payment.order_id, payment.method, payment.amount_cents, payment.paid_at};
}

auto PaymentServices::validate_payment(const models::Payment &payment, int total_order_cents, int total_already_paid) -> void
{
    if (payment.amount_cents <= 0)
    {
        throw exceptions::BadRequestError("Payment amount must be positive");
    }

    if (payment.method == models::PaymentMethod::UNKNOWN)
    {
        throw exceptions::BadRequestError("Invalid payment method");
    }

    int remaining = total_order_cents - total_already_paid;

    if (remaining <= 0)
    {
        throw exceptions::BadRequestError("This order is already fully paid");
    }

    if (payment.amount_cents > remaining)
    {
        auto message = "Payment amount exceeds order total. Remaining: " + std::to_string(remaining) + " cents";
        throw exceptions::BadRequestError(message);
    }
}

auto PaymentServices::create_payment(const models::dto::PaymentCreateDTO &dto) -> models::dto::PaymentResponseDTO
{
    // 1. Centraliza busca de dados (Evita múltiplas chamadas ao DB)
    const int total_order = order_service_->calculate_total_cents(dto.order_id);
    const int total_paid_before = repository_->sum_by_order(dto.order_id);

    // 2. Mapeamento DTO -> Model
    models::Payment payment;
    payment.order_id = dto.order_id;
    payment.amount_cents = dto.amount_cents;
    payment.method = dto.method;
    payment.paid_at = std::nullopt;

    // 3. Validação (Passando os valores já consultados)
    validate_payment(payment, total_order, total_paid_before);

    // 4. Persistência
    repository_->create(payment);

    // 5. Cálculo do novo estado
    int total_paid_after = total_paid_before + payment.amount_cents;
    int remaining = std::max(0, total_order - total_paid_after);

    auto res_dto = to_response_dto(payment);
    res_dto.still_missing = (remaining > 0) ? std::make_optional(remaining) : std::nullopt;

    // 6. Se atingiu o total, atualiza o status do pedido
    if (total_paid_after >= total_order)
    {
        order_service_->mark_order_as_paid(payment.order_id);
    }

    return res_dto;
}

auto PaymentServices::get_payment_by_id(int payment_id) -> models::dto::PaymentResponseDTO
{
    auto payment_opt = repository_->find_by_id(payment_id);
    if (!payment_opt)
    {
        throw exceptions::NotFoundError("Payment not found");
    }

    return to_response_dto(*payment_opt);
}

} // namespace lynx::services