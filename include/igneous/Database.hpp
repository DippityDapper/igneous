#pragma once

#include <optional>
#include <unordered_map>
#include <vector>

#include "SQLiteCpp/SQLiteCpp.h"

namespace Engine
{
    class Database
    {
    public:
        static std::unique_ptr<SQLite::Database> Open(const std::string& filePath);
        static bool Execute(SQLite::Database* db, const std::string& query);
        static std::vector<std::unordered_map<std::string, std::string>> Query(SQLite::Database* db, const std::string& query);

    private:
        static std::string ColumnToString(const SQLite::Column& column);
    };
}