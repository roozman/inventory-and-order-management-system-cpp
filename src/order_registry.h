#ifndef STORE_ORDER_REGISTRY_H
#define STORE_ORDER_REGISTRY_H

#include "order.h"
#include <vector>
#include "customer.h"
#include "inventory.h"
#include <string>

// Requires an existing customer. Generates a new ID and leaves orders unchanged
// if the customer is missing or the ID range is exhausted.
void addDraftOrder(std::vector<Customer>& customersList, std::vector<Order>& ordersList, const std::string& customerId);

// Returns the stored order or nullptr. Adding to the vector can invalidate the pointer.
Order* getOrderById(std::vector<Order>& ordersList, const std::string& orderId);

// Uses the highest existing ID, regardless of vector order; throws when O999999 is used.
uint32_t findNextOrderId(const std::vector<Order>& ordersList);

// Formats 1 through 999999 as O000001 through O999999; rejects values outside that range.
std::string generateOrderID(std::uint32_t id);

void addItemToOrder(std::vector<Order>& ordersList, std::vector<Product>& productsList, const std::string& orderId,
    const std::string& sku, int quantity);

void removeItemFromOrder(std::vector<Order>& ordersList, const std::string& orderId, const std::string& sku);

#endif //STORE_ORDER_REGISTRY_H
