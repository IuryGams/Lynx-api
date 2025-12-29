#include <utils/time/time_utils.h>

namespace utils::time
{
auto time_point_to_string(const std::chrono::system_clock::time_point &tp) -> std::string
{
    auto tt = std::chrono::system_clock::to_time_t(tp);
    std::tm tm{};
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tm, &tt); // Windows
#else
    localtime_r(&tt, &tm); // Linux/macOS
#endif
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

auto string_to_time_point(const std::string &s) -> std::chrono::system_clock::time_point
{
    std::tm tm{};
    std::istringstream ss(s);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    auto tt = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(tt);
}
} // namespace utils::time
