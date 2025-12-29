#pragma once

#include "repository/database/sqlite/sqlite_base_repository.h"
#include "repository/interfaces/interface_payment.h"

namespace lynx::repository
{

class PaymentRepository final : public interface::IPaymentRepository, protected SQLiteBaseRepository
{
public:
    PaymentRepository();

    auto create(models::Payment &Payment) -> void override;
    auto find_by_id(int id) -> std::optional<models::Payment> override;
    auto find_all() -> std::vector<models::Payment> override;
    auto sum_by_order(int order_id) -> int override;
    auto update(const int &id, const models::Payment &Payment) -> void override;
    auto mark_as_paid(int payment_id, const std::string &paid_at) -> void override;
    auto remove(int id) -> void override;
};

} // namespace lynx::repository
