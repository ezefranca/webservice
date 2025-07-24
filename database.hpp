#pragma once
#include <string>
#include <vector>

struct Item {
    int id;
    std::string name;
};

bool initDatabase();
int createItem(const std::string& name);
std::vector<Item> getAllItems();
Item getItemById(int id);
bool updateItem(int id, const std::string& name);
bool deleteItem(int id);