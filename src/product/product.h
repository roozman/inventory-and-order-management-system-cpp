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
    bool archived_;
    int discount_;

    // A valid name contains at least one non-whitespace character.
    static bool productNameValidator(const std::string& name);
    // Throws std::logic_error before any change to an archived product.
    void ensureActive() const;

public:
    // Requires a SKU from P001 to PFFF, a non-blank name, and nonnegative numeric values.
    // Throws std::invalid_argument when any requirement is not met.
    Product(std::string sku, std::string productName,std::int64_t unitPrice, int stockQuantity, int reorderLevel);

    std::string getSku() const;
    std::string getProductName() const;
    std::int64_t getUnitPrice() const;
    int getStockQuantity() const;
    int getReorderLevel() const;
    int getDiscount() const;

    // Edits require an active product; archived products throw std::logic_error.
    // Invalid names and negative values throw std::invalid_argument.
    void setProductName(const std::string& productName);
    void setUnitPrice(std::int64_t unitPrice);
    void setReorderLevel(int reorderLevel);
    void setDiscount(int discount);

    // Amount must be positive. Increasing checks for int overflow; decreasing
    // rejects an amount greater than the current stock.
    void increaseStock(int amount);
    void decreaseStock(int amount);

    // Stock at or below the reorder level counts as low stock.
    bool isLowStock() const;
    bool isArchived() const;

    // Prevents ordinary edits and manual stock changes; cancellation may still restore stock.
    // Archiving it again throws std::logic_error.
    void archive();

    // Restores a positive quantity on order cancellation, including for archived products.
    // Checks overflow and preserves archive status.
    void restoreStock(int amount);
};

#endif //STORE_PRODUCT_H
