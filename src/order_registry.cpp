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

void validateOrderForConfirmation(const Order &order, const std::vector<Product> &products) {

    if (order.getOrderStatus() != OrderStatus::Draft) {
        throw std::logic_error("Cannot confirm order " + order.getId() + ": only Draft orders can be confirmed.");
    }
    if (order.getItems().empty()) {
        throw std::logic_error("Cannot confirm order " + order.getId() + ": the order has no items.");
    }
    
    for (const auto& orderItem : order.getItems()) {

        const Product* foundItemInInventory = getProductBySKU(products, orderItem.getProductSKU());

        if (!foundItemInInventory) {
            throw std::invalid_argument("Cannot confirm order " + order.getId() + ": product "
                + orderItem.getProductSKU() + " was not found in inventory.");
        }

        if (foundItemInInventory->isArchived()) {
            throw std::logic_error("Cannot confirm order " + order.getId()
                + " : product " + foundItemInInventory->getSku() + " is archived.");
        }

        if (orderItem.getOrderQuantity() > foundItemInInventory->getStockQuantity()) {
            throw std::logic_error("Cannot confirm order " + order.getId() + ": insufficient stock for product "
                + orderItem.getProductSKU() + ". Requested: " + std::to_string(orderItem.getOrderQuantity())
                + "; available: " + std::to_string(foundItemInInventory->getStockQuantity()) + ".");
        }
    }
}

void confirmOrder(std::vector<Order> &ordersList, std::vector<Product> &products, const std::string &orderId) {
    Order* order = getOrderById(ordersList, orderId);
    if (!order) {
        throw std::invalid_argument("Order " + orderId + " does not exist.");
    }

    order->confirm(products);
}

void cancelDraftOrder(std::vector<Order> &ordersList, const std::string &orderId) {
    Order* order = getOrderById(ordersList, orderId);

    if (!order) {
        throw std::invalid_argument("Order " + orderId + " does not exist.");
    }

    order->cancelDraft();
}

void cancelConfirmedOrder(std::vector<Order> &ordersList, const std::string &orderId, std::vector<Product> &products) {
    Order* order = getOrderById(ordersList, orderId);

    if (!order) {
        throw std::invalid_argument("Order " + orderId + " does not exist.");
    }

    order->cancelConfirmed(products);
}
