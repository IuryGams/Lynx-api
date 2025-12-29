#pragma once

#include <string>
#include "models/product.h"

namespace lynx::models::dto
{
    struct ProductCreateDTO
    {
        std::string name;
        Category category;
        int price_cents;
        bool active = true;
    };

    struct ProductResponseDTO
    {
        int id;
        std::string name;
        Category category;
        int price_cents;
        bool active;
    };
}