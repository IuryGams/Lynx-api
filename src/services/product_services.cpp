#include "services/product_services.h"
#include "errors/http_handle_error.h"
#include <stdexcept>

namespace lynx::services
{

ProductServices::ProductServices(std::shared_ptr<repository::interface::IProductRepository> repository)
    : repository_(repository)
{
}

auto ProductServices::validate_product(const models::Product &product) -> void
{
    if (product.name.empty())
        throw exceptions::BadRequestError("Product name cannot be empty");

    if (product.price_cents < 0)
        throw exceptions::BadRequestError("Product price cannot be negative");

    switch (product.category)
    {
    case models::Category::KITCHEN:
    case models::Category::ELECTRONICS:
    case models::Category::HOME:
    case models::Category::CLEANING:
    case models::Category::FOOD:
    case models::Category::BEVERAGES:
    case models::Category::PERSONAL_CARE:
    case models::Category::PETS:
    case models::Category::TOOLS:
    case models::Category::OFFICE:
    case models::Category::TOYS:
    case models::Category::CLOTHING:
    case models::Category::OTHER:
        break; // categoria válida
    default:
        throw exceptions::BadRequestError("Invalid product category");
    }
}

auto ProductServices::to_response_dto(const models::Product &product) -> models::dto::ProductResponseDTO
{
    return models::dto::ProductResponseDTO{product.id, product.name, product.category, product.price_cents, product.active};
}

auto ProductServices::from_create_dto(const models::dto::ProductCreateDTO &dto) -> models::Product
{
    models::Product product;
    product.name = dto.name;
    product.category = dto.category;
    product.price_cents = dto.price_cents;
    product.active = dto.active;
    return product;
}

auto ProductServices::create_product(const models::dto::ProductCreateDTO &dto) -> models::dto::ProductResponseDTO
{
    models::Product product = from_create_dto(dto);
    validate_product(product);

    repository_->create(product);

    return to_response_dto(product);
}

auto ProductServices::get_product_by_id(const int &id) -> models::dto::ProductResponseDTO
{
    auto product_opt = repository_->find_product_by_id(id);
    if (!product_opt.has_value())
    {
        throw exceptions::NotFoundError("Product not found");
    }

    return to_response_dto(*product_opt);
}

auto ProductServices::get_all_products(const models::ProductFilters &filters) -> std::vector<models::dto::ProductResponseDTO>
{
    auto products = repository_->find_all(filters);
    std::vector<models::dto::ProductResponseDTO> result;
    result.reserve(products.size());

    for (const auto &product : products)
        result.push_back(to_response_dto(product));

    return result;
}

auto ProductServices::update_product(int product_id, const models::dto::ProductCreateDTO &dto) -> models::dto::ProductResponseDTO
{
    auto product_opt = repository_->find_product_by_id(product_id);
    if (!product_opt)
    {
        throw exceptions::NotFoundError("Product not found");
    }

    models::Product updated = *product_opt;
    updated.name = dto.name;
    updated.category = dto.category;
    updated.price_cents = dto.price_cents;
    updated.active = dto.active;

    validate_product(updated);

    repository_->update(product_id, updated);

    return to_response_dto(updated);
}

} // namespace lynx::services