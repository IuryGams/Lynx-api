#pragma once

#include "models/payment.h"
#include <chrono>
#include <optional>
#include <string>


namespace lynx::models::dto
{
struct PaymentCreateDTO
{
    int order_id;
    PaymentMethod method;
    int amount_cents;
};

struct PaymentResponseDTO
{
    int id;
    int order_id;
    PaymentMethod method;
    int amount_cents;
    std::optional<std::chrono::system_clock::time_point> paid_at;
    std::optional<int> still_missing;
};
} // namespace lynx::models::dto