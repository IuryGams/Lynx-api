#pragma once

#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace lynx::models
{

enum class Category
{
    KITCHEN,       // Utensílios de cozinha
    ELECTRONICS,   // Eletrônicos
    HOME,          // Casa e decoração
    CLEANING,      // Produtos de limpeza
    FOOD,          // Alimentos
    BEVERAGES,     // Bebidas
    PERSONAL_CARE, // Higiene pessoal
    PETS,          // Produtos para pets
    TOOLS,         // Ferramentas
    OFFICE,        // Escritório
    TOYS,          // Brinquedos
    CLOTHING,      // Roupas
    OTHER          // Categoria genérica
};

struct Product
{
    int id;
    std::string name;
    Category category;
    int price_cents;
    bool active = true;
};

struct ProductFilters
{
    std::optional<Category> category;
    std::optional<bool> active;
    std::optional<int> min_price_cents;
    std::optional<int> max_price_cents;
};

} // namespace lynx::models