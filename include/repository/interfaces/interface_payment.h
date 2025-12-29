#pragma once

#include "models/payment.h"
#include <optional>
#include <vector>

namespace lynx::repository::interface
{

class IPaymentRepository
{
public:
    virtual ~IPaymentRepository() = default;

    virtual auto create(models::Payment &payment) -> void = 0;
    virtual auto find_by_id(int id) -> std::optional<models::Payment> = 0;
    virtual auto find_all() -> std::vector<models::Payment> = 0;
    virtual auto sum_by_order(int order_id) -> int = 0;
    virtual auto mark_as_paid(int payment_id, const std::string &paid_at) -> void = 0;
    virtual auto update(const int &id, const models::Payment &payment) -> void = 0;
    virtual auto remove(int id) -> void = 0;
};
} // namespace lynx::repository::interface