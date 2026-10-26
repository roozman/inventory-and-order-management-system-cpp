#ifndef STORE_INVENTORY_H
#define STORE_INVENTORY_H

#include "product.h"
#include <string>
#include <vector>
#include <cstdint>

// Returns the stored product or nullptr if absent. Adding to the vector may
// invalidate a previously returned pointer.
Product* getProductBySKU(std::vector<Product>& products, const std::string& sku);
const Product* getProductBySKU(const std::vector<Product>& products, const std::string& sku);

// Throws std::invalid_argument if the SKU is already registered, including
// when the existing product is archived.
void addProduct(std::vector<Product>& products, const Product& product);

// Formats IDs 1 through 0xFFF as P001 through PFFF; throws std::out_of_range otherwise.
std::string generateSKU(std::uint32_t id);

// Returns one more than the highest SKU, or 1 when empty; throws
// std::out_of_range if PFFF has already been assigned.
std::uint32_t findNextID(const std::vector<Product>& products);

// Returns active products at or below their reorder level. The returned
// pointers refer to vector elements and may be invalidated by vector changes.
std::vector<const Product*> getLowStockProducts(const std::vector<Product>& products);

#endif //STORE_INVENTORY_H
