#include "order.h"

#include <limits>

#include "customer_registry.h"
#include <stdexcept>
#include "order_registry.h"

Order::Order(std::string orderId, std::string customerId)
    : orderId_{orderId}, customerId_{customerId}
{
    if (orderId.size() != 7 || orderId[0] != 'O' || orderId == "O000000"
        || orderId.substr(1).find_first_not_of("0123456789") != std::string::npos) {
        throw std::invalid_argument("Invalid order ID: expected O000001 through O999999 using decimal digits.");

    }
}

std::string Order::getId() const {
    return orderId_;
}

OrderStatus Order::getOrderStatus() const {
    return orderStatus_;
}

std::string Order::getCustomerId() const {
    return customerId_;
}

const std::vector<OrderItem>& Order::getItems() const {
    return orderItems_;
}

void Order::addItem(const Product &product, int quantity) {
    if (orderStatus_ != OrderStatus::Draft) {
        throw std::logic_error("Order is not in the draft stage.");
    }

    if (!orderItems_.empty()) {
        for (const auto& item : orderItems_) {
            if (item.getProductSKU() == product.getSku()) {
                throw std::invalid_argument("This product is already in the order.");
            }
        }
    }

    OrderItem item(product, quantity);
    orderItems_.push_back(item);
}

void Order::removeItem(const std::string &sku) {
    if (orderStatus_ != OrderStatus::Draft) {
        throw std::logic_error("Can not edit an order when it is not in draft mode.");
    }
    for (auto it = orderItems_.begin(); it != orderItems_.end(); ++it) {
        if (it->getProductSKU() == sku) {
            orderItems_.erase(it);
            return;
        }
    }
    throw std::invalid_argument("This product is not present in the order.");
}

std::int64_t Order::getSubTotal() const {
    int64_t subTotal{0};

    for (const auto& item : orderItems_) {
        if (item.getTotalPrice() > std::numeric_limits<std::int64_t>::max() - subTotal) {
            throw std::overflow_error("Subtotal price overflowed.");
        }
        subTotal += item.getTotalPrice();
    }

    return subTotal;
}

std::int64_t Order::getDiscountAmount() const {
    return 0;
}

std::int64_t Order::getFinalTotal() const {
    return getSubTotal() - getDiscountAmount();
}
