#pragma once

#include <chrono>
#include <utility>
#include <vector>
#include <optional>

namespace lynx::models
{

enum class PaymentMethod
{
    PIX,
    CARD,
    BOLETO,
    UNKNOWN
};

struct Payment
{
    int id;
    int order_id;
    PaymentMethod method;
    int amount_cents;
    std::optional<std::chrono::system_clock::time_point> paid_at;
};

} // namespace lynx::models