#pragma once

#include "models/customers.h"
#include <optional>
#include <vector>

namespace lynx::repository::interface
{

class ICustomerRepository
{
public:
    virtual ~ICustomerRepository() = default;

    virtual auto create(models::Customer &customer) -> void = 0;
    virtual auto find_by_id(int id) -> std::optional<models::Customer> = 0;
    virtual auto find_by_email(const std::string &email) -> std::optional<models::Customer> = 0;
    virtual auto find_all() -> std::vector<models::Customer> = 0;
    virtual auto update(const int &id, const models::Customer &customer) -> void = 0;
    virtual auto remove(int id) -> void = 0;
};
} // namespace lynx::repository::interface