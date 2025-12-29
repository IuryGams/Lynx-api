#pragma once

#include <chrono>
#include <optional>
#include <string>


namespace lynx::models::dto
{
struct CustomerCreateDTO
{
    std::string name;
    std::string email;
};

struct CustomerResponseDTO
{
    int id;
    std::string name;
    std::string email;
    std::chrono::system_clock::time_point created_at;
};

struct CustomerUpdateDTO
{
    std::optional<std::string> name;
    std::optional<std::string> email;
};
} // namespace lynx::models::dto