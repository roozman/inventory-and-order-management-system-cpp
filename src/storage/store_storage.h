#ifndef STORE_STORE_STORAGE_H
#define STORE_STORE_STORAGE_H
#include "../order/order.h"
struct StoreData {
    std::vector<Product> products;
    std::vector<Customer> customers;
    std::vector<Order> orders;
};
// Returns a fully validated dataset. Only an entirely absent dataset is a new store.
StoreData loadStore(const std::string& directory, bool allowMissing = false);
void saveStore(const StoreData& data, const std::string& directory);
void recoverStoreSave(const std::string& directory);
#endif
