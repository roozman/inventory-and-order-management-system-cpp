#include "order_registry.h"
#include "order.h"
#include "customer_registry.h"
#include <stdexcept>
#include <cstdint>
#include <sstream>
#include<iomanip>

Order* getOrderById(std::vector<Order> &ordersList, const std::string &orderId) {
    if (ordersList.empty()) {
        return nullptr;
    }

    for (auto& order : ordersList) {
        if (order.getId() == orderId) {
            return &order;
        }
    }
    return nullptr;
}

void addDraftOrder(std::vector<Customer> &customersList, std::vector<Order> &ordersList, const std::string &customerId) {
    if (getCustomerById(customersList, customerId) == nullptr) {
        throw std::invalid_argument("Failed to create order, customer does not exist.");
    }

    uint32_t id = findNextOrderId(ordersList);
    std::string orderId = generateOrderID(id);

    Order newOrder(orderId, customerId);
    ordersList.push_back(newOrder);
}

uint32_t findNextOrderId(const std::vector<Order> &ordersList) {
    if (ordersList.empty()) {
        return 1;
    }
    uint64_t nextId{0};
    // The last order need not have the largest ID after records are loaded or reordered.
    for (const auto& order : ordersList) {
        const std::string orderId = order.getId();
        if (orderId.size() != 7 || orderId[0] != 'O' || orderId.substr(1).find_first_not_of("0123456789") != std::string::npos) {
            throw std::invalid_argument("Order ID is not valid.");
        }

        std::size_t pos{0};
        auto id = std::stoul(orderId.substr(1), &pos, 10);
        if (pos != 6) {
            throw std::invalid_argument("Invalid decimal digits.");
        }

        if (id >= nextId) {
            nextId = static_cast<uint32_t>(id + 1);
        }
    }
    if (nextId > 999999) {
            throw std::out_of_range("Maximum order ID reached.");
    }
    return nextId;
}

std::string generateOrderID(std::uint32_t id) {
    if (id > 999999) {
        throw std::out_of_range("Provided order ID exceeds ID limits.");
    }
    if (id == 0) {
        throw std::invalid_argument("ID 0 is reserved for debug purposes.");
    }

    std::ostringstream oss;
    oss << "O" << std::uppercase << std::setw(6) << std::setfill('0') << id;

    return oss.str();
}

void addItemToOrder(std::vector<Order> &ordersList, std::vector<Product> &productsList, const std::string& orderId, const std::string& sku, int quantity) {
    Product* product = getProductBySKU(productsList, sku);
    if (!product) {
        throw std::invalid_argument("Product does not exist.");
    }
    Order* order = getOrderById(ordersList, orderId);
    if (!order) {
        throw std::invalid_argument("Order does not exist.");
    }

    order->addItem(*product, quantity);
}

void removeItemFromOrder(std::vector<Order> &ordersList, const std::string &orderId, const std::string &sku) {
    Order* order = getOrderById(ordersList, orderId);
    if (!order) {
        throw std::invalid_argument("Order does not exist.");
    }
    order->removeItem(sku);
}
