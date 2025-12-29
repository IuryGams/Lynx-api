#pragma once

#include "models/dtos/dto_customers.h"
#include "repository/interfaces/interface_customer.h"
#include <memory>
#include <string>

namespace lynx::services
{

class CustomerServices
{
private:
    std::shared_ptr<repository::interface::ICustomerRepository> repository_;

    auto validate_customer(const models::Customer &customer) -> void;
    static auto to_response_dto(const models::Customer &customer) -> models::dto::CustomerResponseDTO;

public:
    CustomerServices(std::shared_ptr<repository::interface::ICustomerRepository> repository);

    auto create_customer(const models::dto::CustomerCreateDTO &dto) -> models::dto::CustomerResponseDTO;
    auto get_customer_by_id(const int &id) -> models::dto::CustomerResponseDTO;
    auto get_customer_by_email(const std::string &email) -> models::dto::CustomerResponseDTO;
    auto get_all_customers() -> std::vector<models::dto::CustomerResponseDTO>;
};

} // namespace lynx::services