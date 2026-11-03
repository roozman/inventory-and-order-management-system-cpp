#ifndef STORE_ORDER_STORAGE_H
#define STORE_ORDER_STORAGE_H
#include "../order/order.h"
// Use saveStore for coordinated saving of both files and their referenced collections.
void saveOrders(const std::vector<Order>& orders, const std::string& ordersPath, const std::string& itemsPath);
std::vector<Order> loadOrders(const std::string& ordersPath, const std::string& itemsPath,
                             const std::vector<Customer>& customers, const std::vector<Product>& products);
#endif
