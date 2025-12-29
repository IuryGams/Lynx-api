#include "repository/customer_repository.h"
#include "errors/http_handle_error.h"
#include "utils/time/time_utils.h"
#include <stdexcept>

namespace lynx::repository
{

CustomerRepository::CustomerRepository()
{
}

auto CustomerRepository::create(models::Customer &customer) -> void
{
    const auto db = get_db();
    const char *query = "INSERT INTO customers (name, email, created_at) "
                        "VALUES (?, ?, ?)";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    sqlite3_bind_text(stmt, 1, customer.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, customer.email.c_str(), -1, SQLITE_TRANSIENT);

    const auto created_at_str = utils::time::time_point_to_string(customer.created_at);
    sqlite3_bind_text(stmt, 3, created_at_str.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);

    customer.id = static_cast<int>(sqlite3_last_insert_rowid(db));
}

auto CustomerRepository::find_by_id(int id) -> std::optional<models::Customer>
{
    const auto db = get_db();
    const char *query = "SELECT id, name, email, created_at FROM customers WHERE id = ?";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    sqlite3_bind_int(stmt, 1, id);

    std::optional<models::Customer> result;

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        models::Customer customer;
        customer.id = sqlite3_column_int(stmt, 0);
        customer.name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        customer.email = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));

        std::string created_at_str = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
        customer.created_at = utils::time::string_to_time_point(created_at_str);

        result = customer;
    }

    sqlite3_finalize(stmt);
    return result;
}

auto CustomerRepository::find_by_email(const std::string &email) -> std::optional<models::Customer>
{
    const auto db = get_db();
    const char *query = "SELECT id, name, email, created_at FROM customers WHERE email = ?";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    if (sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    std::optional<models::Customer> result;

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        models::Customer customer;
        customer.id = sqlite3_column_int(stmt, 0);
        customer.name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        customer.email = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));

        std::string created_at_str = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
        customer.created_at = utils::time::string_to_time_point(created_at_str);

        result = customer;
    }

    sqlite3_finalize(stmt);
    return result;
}

auto CustomerRepository::find_all() -> std::vector<models::Customer>
{
    std::vector<models::Customer> result;

    return result;
}

auto CustomerRepository::update(const int &id, const models::Customer &customer) -> void
{
}

auto CustomerRepository::remove(int id) -> void
{
}

} // namespace lynx::repository
