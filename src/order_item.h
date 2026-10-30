#ifndef STORE_ORDER_ITEM_H
#define STORE_ORDER_ITEM_H

#include <cstdint>
#include <string>

#include "product.h"

class OrderItem {
private:
    std::string productSKU_;
    std::string productName_;
    std::int64_t unitPrice_;
    int quantity_;
    int discount_;
    std::int64_t discountAmount_;
    std::int64_t totalPrice_;
    std::int64_t finalTotal_;

public:
    // Snapshots the product SKU, name, base unit price, and discount percentage.
    // Later product edits cannot change this line, even while the order is Draft.
    // Quantity must be positive; gross subtotal overflow is rejected.
    // All amounts use integer rials; the line discount is rounded down.
    OrderItem(const Product& product, int quantity);
    std::string getProductSKU() const;
    std::string getProductName() const;
    std::int64_t getProductUnitPrice() const;
    int getOrderQuantity() const;
    // Returns the captured percentage (0 through 100), not a monetary amount.
    int getDiscount() const;
    // Gross line subtotal before discount: captured unit price times quantity.
    std::int64_t getTotalPrice() const;
    std::int64_t getDiscountAmount() const;
    // Gross line subtotal minus the rounded line discount.
    std::int64_t getFinalTotal() const;
};

#endif //STORE_ORDER_ITEM_H
