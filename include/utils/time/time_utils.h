#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

namespace utils::time
{

auto time_point_to_string(const std::chrono::system_clock::time_point &tp) -> std::string;
auto string_to_time_point(const std::string &s) -> std::chrono::system_clock::time_point;
} // namespace utils::time