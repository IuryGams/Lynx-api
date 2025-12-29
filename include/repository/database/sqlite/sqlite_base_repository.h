#pragma once
#include "database/SQLite_database.h"
#include <sqlite3.h>

namespace lynx::repository
{

class SQLiteBaseRepository
{
protected:
    auto get_db() const -> sqlite3 *
    {
        return lynx::database::SQLiteDatabase::get_instance().get_connection();
    }
};

} // namespace lynx::repository
