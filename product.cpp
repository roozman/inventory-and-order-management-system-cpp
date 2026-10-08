#include "product.h"
#include <string>
#include <cstdint>
#include <stdexcept>

Product::Product(std::string sku,
                 std::string productName,
                 const std::int64_t unitPrice,
                 const int stockQuantity,
                 const int reorderLevel
)
    : sku_{sku}, productName_{productName}, unitPrice_{unitPrice}, stockQuantity_{stockQuantity}, reorderLevel_{reorderLevel} {
    if (productName_.empty()) {
        throw std::invalid_argument("product name cannot be empty.");
    }
    if (const auto firstNonWhitespace = productName_.find_first_not_of(" \t\n\r\f\v"); firstNonWhitespace == std::string::npos) {
        throw std::invalid_argument("product name cannot be empty.");
    }
    if (unitPrice_ < 0) {
        throw std::invalid_argument("negative unit price not allowed.");
    }
    if (stockQuantity_ < 0) {
        throw std::invalid_argument("negative stock quantity not allowed.");
    }
    if (reorderLevel_ < 0) {
        throw std::invalid_argument("negative reorder level not allowed.");
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

