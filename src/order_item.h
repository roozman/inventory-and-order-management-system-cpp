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
    std::int64_t totalPrice_;

public:
    // Copies the product identity, name, and price so later product edits cannot
    // change this line. Quantity must be positive; subtotal overflow is rejected.
    OrderItem(const Product& product, int quantity);
    std::string getProductSKU() const;
    std::string getProductName() const;
    std::int64_t getProductUnitPrice() const;
    int getOrderQuantity() const;
    std::int64_t getTotalPrice() const;
};

#endif //STORE_ORDER_ITEM_H
