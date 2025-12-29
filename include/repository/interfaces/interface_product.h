#pragma once

#include "models/product.h"
#include <optional>
#include <vector>

namespace lynx::repository::interface
{

class IProductRepository
{
public:
    virtual ~IProductRepository() = default;

    virtual auto create(models::Product &product) -> void = 0;
    virtual auto find_product_by_id(int id) -> std::optional<models::Product> = 0;
    virtual auto find_all(const models::ProductFilters &filters) -> std::vector<models::Product> = 0;
    virtual auto update(const int &id, const std::optional<models::Product> &product) -> void = 0;
    virtual auto remove(int id) -> void = 0;
};
} // namespace lynx::repository::interface