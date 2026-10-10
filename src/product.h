#ifndef STORE_PRODUCT_H
#define STORE_PRODUCT_H
#include <string>
#include <cstdint>

class Product {
private:
    std::string sku_;
    std::string productName_;
    std::int64_t unitPrice_;
    int stockQuantity_;
    int reorderLevel_;
public:
    Product(std::string sku, std::string productName,std::int64_t unitPrice, int stockQuantity, int reorderLevel);
    std::string getSku() const;
    std::string getProductName() const;
    std::int64_t getUnitPrice() const;
    int getStockQuantity() const;
    int getReorderLevel() const;
};

#endif //STORE_PRODUCT_H
