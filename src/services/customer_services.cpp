#include "services/customer_services.h"
#include "errors/http_handle_error.h"
#include <stdexcept>

namespace lynx::services
{

CustomerServices::CustomerServices(std::shared_ptr<repository::interface::ICustomerRepository> repository)
    : repository_(repository)
{
}

auto CustomerServices::validate_customer(const models::Customer &customer) -> void
{
    if (customer.name.empty())
    {
        throw exceptions::BadRequestError("Customer name cannot be empty");
    }

    if (customer.email.empty())
    {
        throw exceptions::BadRequestError("Customer email cannot be empty");
    }

    if (customer.email.find('@') == std::string::npos)
    {
        throw exceptions::BadRequestError("Customer email is invalid");
    }

    auto found_customer = repository_->find_by_email(customer.email);
    if (found_customer.has_value())
    {
        throw exceptions::BadRequestError("Email is already registered");
    }
}

auto CustomerServices::to_response_dto(const models::Customer &customer) -> models::dto::CustomerResponseDTO
{
    return models::dto::CustomerResponseDTO{customer.id, customer.name, customer.email, customer.created_at};
}

auto CustomerServices::create_customer(const models::dto::CustomerCreateDTO &dto) -> models::dto::CustomerResponseDTO
{
    models::Customer customer;
    customer.name = dto.name;
    customer.email = dto.email;
    customer.created_at = std::chrono::system_clock::now();

    validate_customer(customer);
    repository_->create(customer);

    return to_response_dto(customer);
}

auto CustomerServices::get_customer_by_id(const int &id) -> models::dto::CustomerResponseDTO
{
    auto customer_opt = repository_->find_by_id(id);

    if (!customer_opt)
    {
        throw exceptions::NotFoundError("Customer not found");
    }

    return to_response_dto(customer_opt.value());
}

auto CustomerServices::get_customer_by_email(const std::string &email) -> models::dto::CustomerResponseDTO
{
    auto customer_opt = repository_->find_by_email(email);
    if (!customer_opt.has_value())
    {
        throw exceptions::CustomError("Customer not found");
    }

    return to_response_dto(customer_opt.value());
}

auto CustomerServices::get_all_customers() -> std::vector<models::dto::CustomerResponseDTO>
{
    auto customers = repository_->find_all();
    std::vector<models::dto::CustomerResponseDTO> result;
    for (const auto &c : customers)
        result.push_back(to_response_dto(c));
    return result;
}

} // namespace lynx::services