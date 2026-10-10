#include "inventory.h"
#include <stdexcept>

Product* getProductBySKU(std::vector<Product>& products, const std::string& sku) {
    for (auto& product : products) {
        if (product.getSku() == sku) {
            return &product;
        }
    }
    return nullptr;
}

void addProduct(std::vector<Product> &products, const Product &product) {
    if (getProductBySKU(products, product.getSku())) {
        throw std::invalid_argument("there exists a product with matching SKU.");
    }
    products.push_back(product);
}