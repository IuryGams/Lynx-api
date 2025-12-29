#include "repository/order_item_repository.h"
#include "errors/http_handle_error.h"

namespace lynx::repository
{

OrderItemRepository::OrderItemRepository() {};

auto OrderItemRepository::create(const models::OrderItem &item) -> void
{
    const auto db = get_db();
    const char *query = "INSERT INTO order_items (order_id, product_id, quantity, unit_price_cents) "
                        "VALUES (?, ?, ?, ?)";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    sqlite3_bind_int(stmt, 1, item.order_id);
    sqlite3_bind_int(stmt, 2, item.product_id);
    sqlite3_bind_int(stmt, 3, item.quantity);
    sqlite3_bind_int(stmt, 4, item.unit_price_cents);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

auto OrderItemRepository::find_by_order_id(int &order_id) -> std::vector<models::OrderItem>
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
} // namespace lynx::repository
