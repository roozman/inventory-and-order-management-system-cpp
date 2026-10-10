#ifndef STORE_INVENTORY_H
#define STORE_INVENTORY_H

#include "product.h"
#include <string>
#include <vector>

Product* getProductBySKU(std::vector<Product>& products, const std::string& sku);
void addProduct(std::vector<Product>& products, const Product& product);

#endif //STORE_INVENTORY_H
