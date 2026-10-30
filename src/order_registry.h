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

void validateOrderForConfirmation(const Order& order, const std::vector<Product>& products);

void confirmOrder(std::vector<Order>& ordersList, std::vector<Product>& products, const std::string& orderId);

void cancelDraftOrder(std::vector<Order>& ordersList, const std::string& orderId);

void cancelConfirmedOrder(std::vector<Order>& ordersList, const std::string& orderId, std::vector<Product>& products);

void completeOrder(std::vector<Order>& ordersList, const std::string& orderId);

// Returned pointers are read-only views; changing the orders vector may invalidate them.
std::vector<const Order*> getCustomerOrders(const std::vector<Order>& ordersList, const std::string& customerId);

struct SalesSummary {
    std::size_t completedOrders{0};
    std::int64_t subtotal{0};
    std::int64_t discount{0};
    std::int64_t revenue{0};
};

// Only Completed orders count as sales. Throws std::overflow_error if any sum overflows.
SalesSummary getSalesSummary(const std::vector<Order>& ordersList);

#endif //STORE_ORDER_REGISTRY_H
