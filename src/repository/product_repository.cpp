#include "repository/product_repository.h"
#include "errors/http_handle_error.h"
#include "utils/convert.h"
#include <stdexcept>

#include <iostream>

namespace lynx::repository
{

ProductRepository::ProductRepository()
{
}

auto ProductRepository::create(models::Product &product) -> void
{
    const auto db = get_db();
    const char *query = "INSERT INTO products (name, category, price_cents, active) VALUES (?, ?, ?, ?)";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    sqlite3_bind_text(stmt, 1, product.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, utils::category_to_string(product.category).c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, product.price_cents);
    sqlite3_bind_int(stmt, 4, product.active ? 1 : 0);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);

    product.id = static_cast<int>(sqlite3_last_insert_rowid(db));
}

auto ProductRepository::find_product_by_id(int id) -> std::optional<models::Product>
{
    const auto db = get_db();
    const char *query = "SELECT id, name, category, price_cents, active FROM products WHERE id = ?";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    sqlite3_bind_int(stmt, 1, id);

    std::optional<models::Product> result;

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        models::Product product;
        product.id = sqlite3_column_int(stmt, 0);
        product.name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));

        std::string category_str = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
        product.category = utils::string_to_category(category_str);

        product.price_cents = sqlite3_column_int(stmt, 3);
        product.active = sqlite3_column_int(stmt, 4) != 0;

        result = product;
    }

    sqlite3_finalize(stmt);
    return result;
}

auto ProductRepository::find_all(const models::ProductFilters &filters) -> std::vector<models::Product>
{
    const auto db = get_db();
    std::string query = "SELECT id, name, category, price_cents, active FROM products WHERE 1=1";
    std::vector<std::pair<int, int>> int_params;
    std::vector<std::string> string_params;

    if (filters.category.has_value())
    {
        query += " AND category = ?";
        string_params.push_back(utils::category_to_string(filters.category.value()));
    }
    if (filters.active.has_value())
    {
        query += " AND active = ?";
        int_params.emplace_back(filters.active.value() ? 1 : 0, 0);
    }
    if (filters.min_price_cents.has_value())
    {
        query += " AND price_cents >= ?";
        int_params.emplace_back(filters.min_price_cents.value(), 0);
    }
    if (filters.max_price_cents.has_value())
    {
        query += " AND price_cents <= ?";
        int_params.emplace_back(filters.max_price_cents.value(), 0);
    }

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        throw exceptions::InternalServerError(sqlite3_errmsg(db));

    int bind_index = 1;
    for (const auto &s : string_params)
        sqlite3_bind_text(stmt, bind_index++, s.c_str(), -1, SQLITE_TRANSIENT);
    for (auto &p : int_params)
        sqlite3_bind_int(stmt, bind_index++, p.first);

    std::vector<models::Product> result;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        models::Product product;
        product.id = sqlite3_column_int(stmt, 0);
        product.name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        product.category = utils::string_to_category(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2)));
        product.price_cents = sqlite3_column_int(stmt, 3);
        product.active = sqlite3_column_int(stmt, 4) != 0;
        result.push_back(product);
    }

    sqlite3_finalize(stmt);
    return result;
}

auto ProductRepository::update(const int &id, const std::optional<models::Product> &product) -> void
{
    if (!product)
    {
        throw exceptions::BadRequestError("No fields to update");
    }

    const auto db = get_db();

    const char *query = R"sql(
        UPDATE products
        SET 
            name = COALESCE(?, name),
            category = COALESCE(?, category),
            price_cents = COALESCE(?, price_cents),
            active = COALESCE(?, active)
        WHERE id = ?
    )sql";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    // Bind de cada campo, passando NULL se não quiser atualizar
    sqlite3_bind_text(stmt, 1, product->name.empty() ? nullptr : product->name.c_str(), -1, SQLITE_TRANSIENT);

    auto cat_str = utils::category_to_string(product->category);
    sqlite3_bind_text(stmt, 2, cat_str.empty() ? nullptr : cat_str.c_str(), -1, SQLITE_TRANSIENT);

    sqlite3_bind_int(stmt, 3, product->price_cents);
    sqlite3_bind_int(stmt, 4, product->active ? 1 : 0);

    sqlite3_bind_int(stmt, 5, id);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

auto ProductRepository::remove(int id) -> void
{
    const auto db = get_db();
    const char *query = "DELETE FROM products WHERE id = ?";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK)
        throw exceptions::InternalServerError(sqlite3_errmsg(db));

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        throw exceptions::InternalServerError(sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

} // namespace lynx::repository
