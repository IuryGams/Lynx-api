#include "database/SQLite_database.h"
#include "errors/http_handle_error.h"
#include <stdexcept>

namespace lynx::database
{

SQLiteDatabase::SQLiteDatabase()
    : database_(nullptr)
{
    const std::string database_path = "C:\\Dev\\Lynx-api\\data\\lynx.db";

    if (sqlite3_open(database_path.c_str(), &database_) != SQLITE_OK)
    {
        std::string error = sqlite3_errmsg(database_);
        sqlite3_close(database_);

        throw exceptions::InternalServerError("Erro ao abrir SQLite: " + error);
    }
}

SQLiteDatabase::~SQLiteDatabase()
{
    if (database_)
    {
        sqlite3_close(database_);
    };
}

auto SQLiteDatabase::get_instance() -> SQLiteDatabase &
{
    static SQLiteDatabase instance;

    return instance;
}

auto SQLiteDatabase::get_connection() const -> sqlite3 *
{
    return database_;
}

} // namespace lynx::database
