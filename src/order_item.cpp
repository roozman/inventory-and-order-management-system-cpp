#include "order_item.h"

#include <limits>
#include <stdexcept>

OrderItem::OrderItem(const Product &product, int quantity)
    : productSKU_{product.getSku()}, productName_{product.getProductName()}, unitPrice_{product.getUnitPrice()},
     quantity_{quantity}, discount_{product.getDiscount()}, discountAmount_{0}, totalPrice_{0}, finalTotal_{0}
{
    if (quantity <= 0) {
        throw std::invalid_argument("Order quantity must be a non-zero positive integer.");
    }
    // Check before multiplication to avoid signed integer overflow.
    if (unitPrice_ > std::numeric_limits<std::int64_t>::max() / quantity) {
        throw std::overflow_error("Line subtotal overflow.");
    }
    totalPrice_ = unitPrice_ * quantity;

    // Split into whole hundreds and a remainder to avoid gross * percentage overflow.
    // Product guarantees 0-100%; integer division rounds the discount down to rials.
    discountAmount_ = (totalPrice_ / 100) * discount_ + ((totalPrice_ % 100) * discount_) / 100;

    finalTotal_ = totalPrice_ - discountAmount_;
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

std::int64_t OrderItem::getDiscountAmount() const {
    return discountAmount_;
}

std::int64_t OrderItem::getFinalTotal() const {
    return finalTotal_;
}

int OrderItem::getDiscount() const {
    return discount_;
}
