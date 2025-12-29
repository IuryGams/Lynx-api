#include "repository/order_repository.h"
#include "errors/http_handle_error.h"
#include "utils/convert.h"
#include "utils/time/time_utils.h"
#include <map>

namespace lynx::repository
{

OrderRepository::OrderRepository()
{
}

auto OrderRepository::create(models::Order &order) -> void
{
    const auto db = get_db();

    sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

    try
    {
        // 1. Insert order
        const char *order_query = "INSERT INTO orders (customer_id, status, created_at) VALUES (?, ?, ?)";

        sqlite3_stmt *order_stmt = nullptr;
        if (sqlite3_prepare_v2(db, order_query, -1, &order_stmt, nullptr) != SQLITE_OK)
            throw std::runtime_error(sqlite3_errmsg(db));

        sqlite3_bind_int(order_stmt, 1, order.customer_id);
        sqlite3_bind_text(order_stmt, 2, utils::order_status_to_string(order.status).c_str(), -1, SQLITE_TRANSIENT);

        auto created_at_str = utils::time::time_point_to_string(order.created_at);

        sqlite3_bind_text(order_stmt, 3, created_at_str.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(order_stmt) != SQLITE_DONE)
            throw std::runtime_error(sqlite3_errmsg(db));

        sqlite3_finalize(order_stmt);

        // 2. Get generated id
        order.id = static_cast<int>(sqlite3_last_insert_rowid(db));

        // 3. Insert items
        const char *item_query = "INSERT INTO order_items (order_id, product_id, quantity, unit_price_cents) "
                                 "VALUES (?, ?, ?, ?)";

        for (const auto &item : order.items)
        {
            sqlite3_stmt *item_stmt = nullptr;
            if (sqlite3_prepare_v2(db, item_query, -1, &item_stmt, nullptr) != SQLITE_OK)
                throw std::runtime_error(sqlite3_errmsg(db));

            sqlite3_bind_int(item_stmt, 1, order.id);
            sqlite3_bind_int(item_stmt, 2, item.product_id);
            sqlite3_bind_int(item_stmt, 3, item.quantity);
            sqlite3_bind_int(item_stmt, 4, item.unit_price_cents);

            if (sqlite3_step(item_stmt) != SQLITE_DONE)
                throw std::runtime_error(sqlite3_errmsg(db));

            sqlite3_finalize(item_stmt);
        }

        sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
    }
    catch (...)
    {
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        throw exceptions::InternalServerError("Failed to create order");
    }
}

auto OrderRepository::find_by_id(int id) -> std::optional<models::Order>
{
    auto const db = get_db();
    const char *query = R"sql(
        SELECT o.id AS order_id, o.customer_id, o.status, o.created_at,
               i.id AS item_id, i.product_id, i.quantity, i.unit_price_cents
        FROM orders o
        LEFT JOIN order_items i ON o.id = i.order_id
        WHERE o.id = ?
    )sql";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK)
        throw exceptions::InternalServerError(sqlite3_errmsg(db));

    sqlite3_bind_int(stmt, 1, id);

    std::optional<models::Order> result;
    models::Order order{};
    bool order_created = false;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        if (!order_created)
        {
            order.id = sqlite3_column_int(stmt, 0);
            order.customer_id = sqlite3_column_int(stmt, 1);

            std::string status_str = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
            order.status = utils::string_to_order_status(status_str);

            std::string created_at_str = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
            order.created_at = utils::time::string_to_time_point(created_at_str);

            order_created = true;
        }

        // Preenche item somente se existir
        if (sqlite3_column_type(stmt, 4) != SQLITE_NULL) // item_id
        {
            models::OrderItem item{};
            item.id = sqlite3_column_int(stmt, 4);
            item.product_id = sqlite3_column_int(stmt, 5);
            item.quantity = sqlite3_column_int(stmt, 6);
            item.unit_price_cents = sqlite3_column_int(stmt, 7);

            order.items.push_back(item);
        }
    }

    sqlite3_finalize(stmt);

    if (order_created)
        result = order;

    return result;
}

auto OrderRepository::find_by_id_with_customer(int id) -> std::optional<models::Order>
{
    const auto db = get_db();
    const char *query = "SELECT "
                        "  o.id AS order_id, "
                        "  o.customer_id, "
                        "  o.status, "
                        "  o.created_at AS order_created_at, "
                        "  c.name AS customer_name, "
                        "  c.email AS customer_email, "
                        "  c.created_at AS customer_created_at "
                        "FROM orders o "
                        "INNER JOIN customers c ON c.id = o.customer_id "
                        "WHERE o.id = ?";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    sqlite3_bind_int(stmt, 1, id);

    std::optional<models::Order> result;

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        models::Order order;

        order.id = sqlite3_column_int(stmt, 0);
        order.customer_id = sqlite3_column_int(stmt, 1);

        std::string status_str = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
        order.status = utils::string_to_order_status(status_str);

        std::string order_created_at = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
        order.created_at = utils::time::string_to_time_point(order_created_at);

        // Customer
        models::Customer customer;
        customer.id = sqlite3_column_int(stmt, 4);

        customer.name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));

        customer.email = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6));

        std::string customer_created_at = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 7));
        customer.created_at = utils::time::string_to_time_point(customer_created_at);

        order.customer = customer;
        result = order;
    }

    sqlite3_finalize(stmt);
    return result;
}

auto OrderRepository::find_all() -> std::vector<models::Order>
{
    const auto db = get_db();
    sqlite3_stmt *stmt_order = nullptr;
    const char *query = "SELECT id, customer_id, status, created_at FROM orders";

    if (sqlite3_prepare_v2(db, query, -1, &stmt_order, nullptr) != SQLITE_OK)
    {
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    std::vector<models::Order> orders;

    while (sqlite3_step(stmt_order) == SQLITE_ROW)
    {
        models::Order order;
        order.id = sqlite3_column_int(stmt_order, 0);
        order.customer_id = sqlite3_column_int(stmt_order, 1);

        std::string status_str = reinterpret_cast<const char *>(sqlite3_column_text(stmt_order, 2));
        order.status = utils::string_to_order_status(status_str);

        std::string created_at_str = reinterpret_cast<const char *>(sqlite3_column_text(stmt_order, 3));
        order.created_at = utils::time::string_to_time_point(created_at_str);

        // --- Busca os itens desse pedido ---
        sqlite3_stmt *stmt_items = nullptr;
        const char *sql_items = "SELECT product_id, quantity, unit_price_cents FROM order_items WHERE order_id = ?";
        if (sqlite3_prepare_v2(db, sql_items, -1, &stmt_items, nullptr) != SQLITE_OK)
            throw exceptions::InternalServerError(sqlite3_errmsg(db));

        sqlite3_bind_int(stmt_items, 1, order.id);

        while (sqlite3_step(stmt_items) == SQLITE_ROW)
        {
            models::OrderItem item;
            item.product_id = sqlite3_column_int(stmt_items, 0);
            item.quantity = sqlite3_column_int(stmt_items, 1);
            item.unit_price_cents = sqlite3_column_int(stmt_items, 2);

            order.items.push_back(item);
        }

        sqlite3_finalize(stmt_items);

        orders.push_back(order);
    }

    sqlite3_finalize(stmt_order);
    return orders;
}

auto OrderRepository::find_all_summary(const std::optional<std::string> &status_filter, const std::optional<int> &customer_id_filter,
                                       const std::optional<int> &limit) -> std::vector<models::OrderSummary>
{
    const auto db = get_db();
    sqlite3_stmt *stmt = nullptr;

    // Query base
    std::string query = R"SQL(
        SELECT
            o.id,
            o.customer_id,
            o.status,
            o.created_at,
            (
                SELECT COALESCE(SUM(oi.quantity * oi.unit_price_cents), 0)
                FROM order_items oi
                WHERE oi.order_id = o.id
            ) AS total_cents,
            (
                SELECT COALESCE(SUM(p.amount_cents), 0)
                FROM payments p
                WHERE p.order_id = o.id
            ) AS total_paid_cents
        FROM orders o
    )SQL";

    // Montar WHERE dinamicamente
    bool hasWhere = false;
    if (status_filter.has_value())
    {
        query += " WHERE o.status = ? ";
        hasWhere = true;
    }
    if (customer_id_filter.has_value())
    {
        query += hasWhere ? " AND o.customer_id = ? " : " WHERE o.customer_id = ? ";
        hasWhere = true;
    }

    query += " ORDER BY o.created_at DESC ";

    // Adicionar LIMIT se houver
    if (limit.has_value())
    {
        query += " LIMIT ? ";
    }

    // Preparar statement
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        throw exceptions::InternalServerError(sqlite3_errmsg(db));

    // Bind dos parâmetros
    int paramIndex = 1;
    if (status_filter.has_value())
    {
        sqlite3_bind_text(stmt, paramIndex++, status_filter->c_str(), -1, SQLITE_TRANSIENT);
    }
    if (customer_id_filter.has_value())
    {
        sqlite3_bind_int(stmt, paramIndex++, *customer_id_filter);
    }
    if (limit.has_value())
    {
        sqlite3_bind_int(stmt, paramIndex++, *limit);
    }

    // Executar query
    std::vector<models::OrderSummary> orders;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        models::OrderSummary order;
        order.id = sqlite3_column_int(stmt, 0);
        order.customer_id = sqlite3_column_int(stmt, 1);

        std::string status_str = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
        order.status = utils::string_to_order_status(status_str);

        std::string created_at_str = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
        order.created_at = utils::time::string_to_time_point(created_at_str);

        order.total_cents = sqlite3_column_int64(stmt, 4);
        order.total_paid_cents = sqlite3_column_int64(stmt, 5);

        orders.push_back(order);
    }

    sqlite3_finalize(stmt);
    return orders;
}

auto OrderRepository::update(const int &id, const models::Order &Order) -> void
{
}

auto OrderRepository::update_status(int order_id, const models::OrderStatus &status) -> void
{
    const auto db = get_db();
    const char *query = "UPDATE orders SET status = ? WHERE id = ?";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    std::string status_str = utils::order_status_to_string(status);

    sqlite3_bind_text(stmt, 1, status_str.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, order_id);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    // Garante que alguma linha foi afetada
    if (sqlite3_changes(db) == 0)
    {
        sqlite3_finalize(stmt);
        throw exceptions::NotFoundError("Order not found: id = " + std::to_string(order_id));
    }

    sqlite3_finalize(stmt);
}

auto OrderRepository::remove(int id) -> void
{
}

/* Order Items */
auto OrderRepository::find_items_by_order_id(int order_id) -> std::vector<models::OrderItem>
{
    const auto db = get_db();
    const char *query = "SELECT id, order_id, product_id, quantity, unit_price_cents "
                        "FROM order_items WHERE order_id = ?";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    sqlite3_bind_int(stmt, 1, order_id);

    std::vector<models::OrderItem> items;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        models::OrderItem item;
        item.id = sqlite3_column_int(stmt, 0);
        item.order_id = sqlite3_column_int(stmt, 1);
        item.product_id = sqlite3_column_int(stmt, 2);
        item.quantity = sqlite3_column_int(stmt, 3);
        item.unit_price_cents = sqlite3_column_int(stmt, 4);

        items.push_back(item);
    }

    sqlite3_finalize(stmt);
    return items;
}

auto OrderRepository::sum_items_total_by_order(int order_id) -> int64_t
{
    const auto db = get_db();
    const char *query = "SELECT COALESCE(SUM(quantity * unit_price_cents), 0)"
                        "FROM order_items WHERE order_id = ?";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    sqlite3_bind_int(stmt, 1, order_id);

    int64_t total = 0;

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        total = sqlite3_column_int(stmt, 0);
    }

    return total;
}

} // namespace lynx::repository
