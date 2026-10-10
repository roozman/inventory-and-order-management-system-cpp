#ifndef STORE_INVENTORY_H
#define STORE_INVENTORY_H

#include "product.h"
#include <string>
#include <vector>
#include <cstdint>

Product* getProductBySKU(std::vector<Product>& products, const std::string& sku);
void addProduct(std::vector<Product>& products, const Product& product);
std::string generateSKU(std::uint32_t id);
std::uint32_t findNextID(const std::vector<Product>& products);

#endif //STORE_INVENTORY_H
