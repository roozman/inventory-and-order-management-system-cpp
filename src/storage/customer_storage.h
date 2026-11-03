#ifndef STORE_CUSTOMER_STORAGE_H
#define STORE_CUSTOMER_STORAGE_H
#include "../customer/customer.h"
#include <vector>
void saveCustomers(const std::vector<Customer>& customers, const std::string& path);
std::vector<Customer> loadCustomers(const std::string& path);
#endif
