#pragma once

#include <sqlite3.h>
#include <string>

namespace lynx::database
{

class SQLiteDatabase
{
private:
    SQLiteDatabase();
    ~SQLiteDatabase();

    sqlite3 *database_;

    SQLiteDatabase(const SQLiteDatabase &) = delete;
    SQLiteDatabase &operator=(const SQLiteDatabase &) = delete;

public:
    static auto get_instance() -> SQLiteDatabase &;
    auto get_connection() const -> sqlite3 *;
};
} // namespace lynx::database