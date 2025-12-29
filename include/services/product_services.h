#pragma once

#include "models/dtos/dto_product.h"
#include "repository/interfaces/interface_product.h"
#include <memory>
#include <string>

namespace lynx::services
{

class ProductServices
{
private:
    std::shared_ptr<repository::interface::IProductRepository> repository_;

    auto validate_product(const models::Product &product) -> void;

    auto to_response_dto(const models::Product &product) -> models::dto::ProductResponseDTO;

    auto from_create_dto(const models::dto::ProductCreateDTO &dto) -> models::Product;

public:
    ProductServices(std::shared_ptr<repository::interface::IProductRepository> repository);

    auto create_product(const models::dto::ProductCreateDTO &dto) -> models::dto::ProductResponseDTO;
    auto get_product_by_id(const int &id) -> models::dto::ProductResponseDTO;
    auto get_all_products(const models::ProductFilters &filters = {}) -> std::vector<models::dto::ProductResponseDTO>;
    auto update_product(int product_id, const models::dto::ProductCreateDTO &dto) -> models::dto::ProductResponseDTO;
};

} // namespace lynx::services