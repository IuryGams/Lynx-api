#include "repository/payment_repository.h"
#include "errors/http_handle_error.h"
#include "utils/convert.h"
#include "utils/time/time_utils.h"
#include <stdexcept>

namespace lynx::repository
{

PaymentRepository::PaymentRepository()
{
}

auto PaymentRepository::create(models::Payment &payment) -> void
{
    const auto db = get_db();
    const char *query = R"(
        INSERT INTO payments (order_id, method, amount_cents, paid_at)
        VALUES (?, ?, ?, ?)
    )";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    sqlite3_bind_int(stmt, 1, payment.order_id);

    auto payment_str = utils::payment_method_to_string(payment.method);
    sqlite3_bind_text(stmt, 2, payment_str.c_str(), -1, SQLITE_TRANSIENT);

    sqlite3_bind_int(stmt, 3, payment.amount_cents);

    if (payment.paid_at.time_since_epoch().count() > 0)
    {
        const auto paid_at_str = utils::time::time_point_to_string(payment.paid_at);
        sqlite3_bind_text(stmt, 4, paid_at_str.c_str(), -1, SQLITE_TRANSIENT);
    }
    else
    {
        sqlite3_bind_null(stmt, 4);
    }

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);

    payment.id = static_cast<int>(sqlite3_last_insert_rowid(db));
}

auto PaymentRepository::find_by_id(int id) -> std::optional<models::Payment>
{
    const auto db = get_db();
    const char *query = "SELECT id, order_id, method, amount_cents, paid_at FROM payments WHERE id = ?";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    sqlite3_bind_int(stmt, 1, id);

    std::optional<models::Payment> result;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        models::Payment payment;
        payment.id = sqlite3_column_int(stmt, 0);
        payment.order_id = sqlite3_column_int(stmt, 1);

        std::string method_str = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
        payment.method = utils::string_to_payment_method(method_str);

        payment.amount_cents = sqlite3_column_int(stmt, 3);

        if (sqlite3_column_type(stmt, 4) != SQLITE_NULL)
        {
            std::string paid_at_str = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
            payment.paid_at = utils::time::string_to_time_point(paid_at_str);
        }

        result = payment;
    }

    sqlite3_finalize(stmt);
    return result;
}

auto PaymentRepository::find_all() -> std::vector<models::Payment>
{
    const auto db = get_db();
    const char *query = "SELECT id, order_id, method, amount_cents, paid_at FROM payments";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    std::vector<models::Payment> payments;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        models::Payment payment;
        payment.id = sqlite3_column_int(stmt, 0);
        payment.order_id = sqlite3_column_int(stmt, 1);

        std::string method_str = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
        payment.method = utils::string_to_payment_method(method_str);

        payment.amount_cents = sqlite3_column_int(stmt, 3);

        if (sqlite3_column_type(stmt, 4) != SQLITE_NULL)
        {
            std::string paid_at_str = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
            payment.paid_at = utils::time::string_to_time_point(paid_at_str);
        }

        payments.push_back(payment);
    }

    sqlite3_finalize(stmt);
    return payments;
}

auto PaymentRepository::sum_by_order(int order_id) -> int
{
    const auto db = get_db();

    const char *query = "SELECT COALESCE(SUM(amount_cents), 0) "
                        "FROM payments "
                        "WHERE order_id = ? ";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    sqlite3_bind_int(stmt, 1, order_id);

    int total_paid = 0;

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        total_paid = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return total_paid;
}

auto PaymentRepository::mark_as_paid(int payment_id, const std::string &paid_at) -> void
{
    const auto db = get_db();

    const char *query = "UPDATE payments SET paid_at = ? WHERE id = ?";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    sqlite3_bind_text(stmt, 1, paid_at.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, payment_id);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

auto PaymentRepository::update(const int &id, const models::Payment &payment) -> void
{
    const auto db = get_db();
    const char *query = R"(
        UPDATE payments
        SET order_id = ?, method = ?, amount_cents = ?, paid_at = ?
        WHERE id = ?
    )";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    sqlite3_bind_int(stmt, 1, payment.order_id);

    auto payment_str = utils::payment_method_to_string(payment.method);
    sqlite3_bind_text(stmt, 2, payment_str.c_str(), -1, SQLITE_TRANSIENT);

    sqlite3_bind_int(stmt, 3, payment.amount_cents);

    if (payment.paid_at.time_since_epoch().count() > 0)
    {
        const auto paid_at_str = utils::time::time_point_to_string(payment.paid_at);
        sqlite3_bind_text(stmt, 4, paid_at_str.c_str(), -1, SQLITE_TRANSIENT);
    }
    else
    {
        sqlite3_bind_null(stmt, 4);
    }

    sqlite3_bind_int(stmt, 5, id);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

auto PaymentRepository::remove(int id) -> void
{
    const auto db = get_db();
    const char *query = "DELETE FROM payments WHERE id = ?";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

} // namespace lynx::repository
