#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <utility>

namespace lynx::models
{

struct Customer
{
    int id;
    std::string name;
    std::string email;
    std::chrono::system_clock::time_point created_at;
};

} // namespace lynx::models