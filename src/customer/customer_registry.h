#ifndef STORE_CUSTOMER_REGISTRY_H
#define STORE_CUSTOMER_REGISTRY_H

#include "customer.h"
#include <vector>
#include <cstdint>

// Returns a pointer to the stored customer, or nullptr if absent.
// Adding to the vector may invalidate a previously returned pointer.
Customer* getCustomerById(std::vector<Customer>& customers, const std::string& id);

// Adds a customer; throws std::invalid_argument if the ID is already registered.
void addCustomer(std::vector<Customer>& customers, const Customer& customer);

// Formats IDs 1 through 0xFFF as C001 through CFFF; rejects IDs outside that range.
std::string generateCustomerId(std::uint32_t id);

// Returns one more than the highest registered ID, or 1 when empty.
// Throws std::out_of_range when CFFF is already registered.
std::uint32_t findNextCustomerId(const std::vector<Customer>& customer);

#endif //STORE_CUSTOMER_REGISTRY_H
