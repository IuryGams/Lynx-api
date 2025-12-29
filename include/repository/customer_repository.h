#pragma once

#include "repository/database/sqlite/sqlite_base_repository.h"
#include "repository/interfaces/interface_customer.h"

namespace lynx::repository
{

class CustomerRepository final : public interface::ICustomerRepository, protected SQLiteBaseRepository
{
public:
    CustomerRepository();

    auto create(models::Customer &customer) -> void override;
    auto find_by_id(int id) -> std::optional<models::Customer> override;
    auto find_by_email(const std::string &email) -> std::optional<models::Customer> override;
    auto find_all() -> std::vector<models::Customer> override;
    auto update(const int &id, const models::Customer &customer) -> void override;
    auto remove(int id) -> void override;
};

} // namespace lynx::repository
