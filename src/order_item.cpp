#include "order_item.h"

#include <limits>
#include <stdexcept>

OrderItem::OrderItem(const Product &product, int quantity)
    : productSKU_{product.getSku()}, productName_{product.getProductName()}, unitPrice_{product.getUnitPrice()},
     quantity_{quantity}, totalPrice_{0}
{
    if (quantity <= 0) {
        throw std::invalid_argument("Order quantity must be a non-zero positive integer.");
    }
    // Check before multiplication to avoid signed integer overflow.
    if (unitPrice_ > std::numeric_limits<std::int64_t>::max() / quantity) {
        throw std::overflow_error("Line subtotal overflow.");
    }
    totalPrice_ = unitPrice_ * quantity;
}

std::string OrderItem::getProductSKU() const {
    return productSKU_;
}

std::string OrderItem::getProductName() const {
    return productName_;
}

std::int64_t OrderItem::getProductUnitPrice() const {
    return unitPrice_;
}

int OrderItem::getOrderQuantity() const {
    return quantity_;
}

std::int64_t OrderItem::getTotalPrice() const {
    return totalPrice_;
}
