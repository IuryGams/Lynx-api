#pragma once

#include "repository/database/sqlite/sqlite_base_repository.h"
#include "repository/interfaces/interface_product.h"

namespace lynx::repository
{

class ProductRepository final : public interface::IProductRepository, protected SQLiteBaseRepository
{
public:
    ProductRepository();

    auto create(models::Product &product) -> void override;
    auto find_product_by_id(int id) -> std::optional<models::Product> override;
    auto find_all(const models::ProductFilters &filters) -> std::vector<models::Product> override;
    auto update(const int &id, const std::optional<models::Product> &product) -> void override;
    auto remove(int id) -> void override;
};

} // namespace lynx::repository
