#include "database.hpp"
#include <sqlite3.h>
#include <iostream>

sqlite3* db = nullptr;

bool initDatabase() {
    if (sqlite3_open("data.db", &db) != SQLITE_OK) return false;
    const char* sql = "CREATE TABLE IF NOT EXISTS items (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT);";
    return sqlite3_exec(db, sql, nullptr, nullptr, nullptr) == SQLITE_OK;
}

int createItem(const std::string& name) {
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "INSERT INTO items(name) VALUES (?);", -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    int id = (int)sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    return id;
}

std::vector<Item> getAllItems() {
    std::vector<Item> items;
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "SELECT id, name FROM items;", -1, &stmt, nullptr);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        items.push_back({sqlite3_column_int(stmt, 0), (const char*)sqlite3_column_text(stmt, 1)});
    }
    sqlite3_finalize(stmt);
    return items;
}

Item getItemById(int id) {
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "SELECT id, name FROM items WHERE id = ?;", -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, id);
    Item item{-1, ""};
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        item.id = sqlite3_column_int(stmt, 0);
        item.name = (const char*)sqlite3_column_text(stmt, 1);
    }
    sqlite3_finalize(stmt);
    return item;
}

bool updateItem(int id, const std::string& name) {
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "UPDATE items SET name = ? WHERE id = ?;", -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, id);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}

bool deleteItem(int id) {
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "DELETE FROM items WHERE id = ?;", -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, id);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}