#ifndef STORE_PRODUCT_STORAGE_H
#define STORE_PRODUCT_STORAGE_H
#include <vector>
#include <string>
#include "../product/product.h"
#include <iosfwd>

void createProductHeader(std::ofstream& file);

void saveProducts(const std::vector<Product>& products, const std::string& filePath);

std::vector<Product> loadProducts(const std::string& filePath);

std::string escapeCsvField(const std::string& text);

void recoverProductSave(const std::string& filePath);

bool readCsvRecord(std::istream& input, std::vector<std::string>& fields);

#endif //STORE_PRODUCT_STORAGE_H
