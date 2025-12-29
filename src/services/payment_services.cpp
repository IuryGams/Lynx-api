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

auto PaymentServices::validate_payment(const models::Payment &payment) -> void
{
    auto order = order_service_->get_order_by_id(payment.order_id);

    int total_order_cents = 0;

    for (const auto &item : order.items)
    {
        total_order_cents += item.quantity * item.unit_price_cents;
    }

    if (payment.amount_cents <= 0)
    {
        throw exceptions::BadRequestError("Payment amount must be positive");
    }

    if (payment.amount_cents > total_order_cents)
    {
        throw exceptions::BadRequestError("Payment amount exceeds order total");
    }

    if (payment.method == models::PaymentMethod::UNKNOWN)
    {
        throw exceptions::BadRequestError("Invalid payment method");
    }
}

auto PaymentServices::create_payment(const models::dto::PaymentCreateDTO &dto) -> models::dto::PaymentResponseDTO
{
    models::Payment payment;
    payment.order_id = dto.order_id;
    payment.method = dto.method;
    payment.amount_cents = dto.amount_cents;
    payment.paid_at = std::chrono::system_clock::now();

    validate_payment(payment);

    /* create the payment in DB */
    repository_->create(payment);

    /* verify that the order has been fully paid */
    int total_paid = repository_->sum_by_order(payment.order_id);
    int order_total = order_service_->calculate_total_cents(payment.order_id);

    if (total_paid >= order_total)
    {
        const auto paid_at = utils::time::time_point_to_string(std::chrono::system_clock::now());

        repository_->mark_as_paid(payment.id, paid_at);
        order_service_->mark_order_as_paid(payment.order_id);
    }

    return to_response_dto(payment);
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