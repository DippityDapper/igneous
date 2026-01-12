#include "igneous/engine/Database.hpp"

#include <iostream>

#include "sqlite3.h"

namespace Engine
{
    std::unique_ptr<SQLite::Database> Database::Open(const std::string& filePath)
    {
        try
        {
            return std::make_unique<SQLite::Database>(
                filePath,
                SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE
            );
        }
        catch (const std::exception& e)
        {
            std::cerr << "Database open error: " << e.what() << std::endl;
            return nullptr;
        }
    }

    bool Database::Execute(SQLite::Database* db, const std::string& query)
    {
        try
        {
            db->exec(query);
            return true;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Query execution error: " << e.what() << std::endl;
            return false;
        }
    }

    std::vector<std::unordered_map<std::string, std::string>> Database::Query(SQLite::Database* db, const std::string& query)
    {
        try
        {
            SQLite::Statement statement(*db, query);
            std::vector<std::unordered_map<std::string, std::string>> results{};

            int columnCount = statement.getColumnCount();

            while (statement.executeStep())
            {
                std::unordered_map<std::string, std::string> row{};

                for (int i = 0; i < columnCount; ++i)
                {
                    std::string columnName = statement.getColumnName(i);
                    row[columnName] = ColumnToString(statement.getColumn(i));
                }

                results.push_back(std::move(row));
            }

            return results;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Query error: " << e.what() << std::endl;
            return {};
        }
    }

    std::string Database::ColumnToString(const SQLite::Column& column)
    {
        if (column.isNull())
        {
            return "";
        }

        switch (column.getType())
        {
        case SQLITE_INTEGER:
            return std::to_string(column.getInt64());
        case SQLITE_FLOAT:
            return std::to_string(column.getDouble());
        case SQLITE_TEXT:
            return column.getText();
        case SQLITE_BLOB:
            return "<BLOB>";
        default:
            return "";
        }
    }
}
