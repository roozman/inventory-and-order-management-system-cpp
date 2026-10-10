#include "product.h"
#include <string>
#include <cstdint>
#include <stdexcept>
#include <limits>

Product::Product(std::string sku,
                 std::string productName,
                 const std::int64_t unitPrice,
                 const int stockQuantity,
                 const int reorderLevel
)
    : sku_{sku}, productName_{productName}, unitPrice_{unitPrice}, stockQuantity_{stockQuantity}, reorderLevel_{reorderLevel} {
    if (sku.size() != 4 || sku[0] != 'P'
        || sku == "P000" || sku.substr(1).find_first_not_of("0123456789ABCDEF") != std::string::npos) {
        throw std::invalid_argument("Invalid SKU: expected P001 through PFFF using uppercase hexadecimal digits.");
    }
    if (!productNameValidator(productName_)) {
        throw std::invalid_argument("Product name cannot be empty or whitespace-only.");
    }
    if (unitPrice_ < 0) {
        throw std::invalid_argument("Unit price cannot be negative.");
    }
    if (stockQuantity_ < 0) {
        throw std::invalid_argument("Stock quantity cannot be negative.");
    }
    if (reorderLevel_ < 0) {
        throw std::invalid_argument("Reorder level cannot be negative.");
    }
}

std::string Product::getSku() const {
    return sku_;
}

std::string Product::getProductName() const {
    return productName_;
}

std::int64_t Product::getUnitPrice() const {
    return unitPrice_;
}

int Product::getStockQuantity() const {
    return stockQuantity_;
}

int Product::getReorderLevel() const {
    return reorderLevel_;
}

bool Product::productNameValidator(const std::string& name) {
    return name.find_first_not_of(" \t\n\r\f\v") != std::string::npos;
}

void Product::setProductName(const std::string& productName) {
    if (!productNameValidator(productName)) {
        throw std::invalid_argument("Product name cannot be empty or whitespace-only.");
    }
    productName_ = productName;
}

void Product::setUnitPrice(const std::int64_t unitPrice) {
    if (unitPrice < 0) {
        throw std::invalid_argument("Unit price cannot be negative.");
    }
    unitPrice_ = unitPrice;
}

void Product::setReorderLevel(const int reorderLevel) {
    if (reorderLevel < 0) {
        throw std::invalid_argument("Reorder level cannot be negative.");
    }
    reorderLevel_ = reorderLevel;
}

void Product::increaseStock(const int amount) {
    if (amount < 1) {
        throw std::invalid_argument("Stock increase must be a positive integer.");
    }
    if (amount > std::numeric_limits<int>::max() - stockQuantity_) {
        throw std::overflow_error("Stock quantity overflow.");
    }
    stockQuantity_ += amount;
}

void Product::decreaseStock(const int amount) {
    if (amount < 1) {
        throw std::invalid_argument("Stock decrease must be a positive integer.");
    }
    if (amount > stockQuantity_) {
        throw std::out_of_range("Cannot decrease stock by an amount larger than the current inventory.");
    }
    stockQuantity_ -= amount;
}

bool Product::isLowStock() const {
    return stockQuantity_ <= reorderLevel_;
}
