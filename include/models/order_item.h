#pragma once

#include <string>
#include <utility>
#include <vector>

namespace lynx::models
{

struct OrderItem
{
    int id;
    int order_id;
    int product_id;
    int quantity;
    int unit_price_cents;
};

} // namespace lynx::models