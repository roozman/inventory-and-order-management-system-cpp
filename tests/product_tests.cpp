#include "../product.h"
#include <iostream>
#include <string>
#include <stdexcept>

std::string testConstructorMatchingValues(std::string sku,
    std::string productName,
    std::int64_t unitPrice,
    int stockQuantity,
    int reorderLevel);
std::string testConstructorEmptyName(std::string sku,
    std::string productName,
    std::int64_t unitPrice,
    int stockQuantity,
    int reorderLevel);
std::string testConstructorWhitespaceOnlyName(std::string sku,
    std::string productName,
    std::int64_t unitPrice,
    int stockQuantity,
    int reorderLevel);
std::string testConstructorNegativeStock(std::string sku,
    std::string productName,
    std::int64_t unitPrice,
    int stockQuantity,
    int reorderLevel);
std::string testConstructorNegativePrice(std::string sku,
    std::string productName,
    std::int64_t unitPrice,
    int stockQuantity,
    int reorderLevel);
std::string testConstructorNegativeReorder(std::string sku,
    std::string productName,
    std::int64_t unitPrice,
    int stockQuantity,
    int reorderLevel);
std::string testConstructorAllZeroValues(std::string sku,
    std::string productName,
    std::int64_t unitPrice,
    int stockQuantity,
    int reorderLevel);
std::string testConstructorNameContainingWhitespace(std::string sku,
    std::string productName,
    std::int64_t unitPrice,
    int stockQuantity,
    int reorderLevel);

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Provide one test name. \n";
        return 1;
    }

    const std::string testName{argv[1]};

    std::string errors;

    if (testName == "valid") {
        errors = testConstructorMatchingValues("fff", "biscuit", 20000, 23, 5);
    }
    else if (testName == "empty_name") {
        errors = testConstructorEmptyName("fff", "",20000, 23, 5 );
    }
    else if (testName == "whitespace_name") {
        errors = testConstructorWhitespaceOnlyName("fff", "   ", 20000, 23, 5);
    }
    else if (testName == "neg_stock") {
        errors = testConstructorNegativeStock("fff", "cake", 20000, -23, 5);
    }
    else if (testName == "neg_price") {
        errors = testConstructorNegativePrice("fff", "chips", -20000, 23, 5);
    }
    else if (testName == "neg_reorder") {
        errors = testConstructorNegativeReorder("fff", "chips", 20000, 23, -5);
    }
    else if (testName == "all_zero_values") {
        errors = testConstructorAllZeroValues("fff", "chips", 0, 0, 0);
    }
    else if (testName == "name_whitespace") {
        errors = testConstructorNameContainingWhitespace("fff", "fish cracker", 10000, 0, 0);
    }
    else {
        std::cerr << "Unknown test name: " << testName << "\n";
        return 1;
    }

    if (!errors.empty()) {
        std::cerr << errors;
        return 1;
    }

    std::cout << "passed.\n";
    return 0;
}

std::string testConstructorMatchingValues(
    std::string sku,
    std::string productName,
    std::int64_t unitPrice,
    int stockQuantity,
    int reorderLevel)
{
    //Testing the values returned by the getters.
    Product product(sku, productName, unitPrice,
                    stockQuantity, reorderLevel);

    std::string errors;

    if (product.getSku() != sku) {
        errors += "SKU mismatch.\n";
    }

    if (product.getProductName() != productName) {
        errors += "Product name mismatch.\n";
    }

    if (product.getUnitPrice() != unitPrice) {
        errors += "Unit price mismatch.\n";
    }

    if (product.getStockQuantity() != stockQuantity) {
        errors += "Stock quantity mismatch.\n";
    }

    if (product.getReorderLevel() != reorderLevel) {
        errors += "Reorder level mismatch.\n";
    }

    return errors;
}

std::string testConstructorEmptyName(
    std::string sku,
    std::string productName,
    std::int64_t unitPrice,
    int stockQuantity,
    int reorderLevel)
{
    std::string errors;
    try {
        Product product(sku, productName, unitPrice,
                stockQuantity, reorderLevel);
    }
    catch (const std::invalid_argument&) {
        return errors;
    }
    errors += "Expected std::invalid_argument, but construction succeeded.";
    return errors;
}

std::string testConstructorWhitespaceOnlyName(
    std::string sku,
    std::string productName,
    std::int64_t unitPrice,
    int stockQuantity,
    int reorderLevel)
{
    std::string errors;
    try {
        Product product(sku, productName, unitPrice,
            stockQuantity, reorderLevel);
    }
    catch (const std::invalid_argument&) {
        return errors;
    }
    errors += "Expected std::invalid_argument, but construction succeeded.";
    return errors;
}

std::string testConstructorNegativeStock(std::string sku,
    std::string productName,
    std::int64_t unitPrice,
    int stockQuantity,
    int reorderLevel) {
    std::string errors;
    try {
        Product product(sku, productName, unitPrice,
            stockQuantity, reorderLevel);
    }
    catch (const std::invalid_argument&) {
        return errors;
    }
    errors += "Expected std::invalid_argument, but construction succeeded.";
    return errors;
}

std::string testConstructorNegativePrice(std::string sku,
    std::string productName,
    std::int64_t unitPrice,
    int stockQuantity,
    int reorderLevel) {
    std::string errors;
    try {
        Product product(sku, productName, unitPrice,
            stockQuantity, reorderLevel);
    }
    catch (const std::invalid_argument&) {
        return errors;
    }
    errors += "Expected std::invalid_argument, but construction succeeded.";
    return errors;
}

std::string testConstructorNegativeReorder(std::string sku,
    std::string productName,
    std::int64_t unitPrice,
    int stockQuantity,
    int reorderLevel) {
    std::string errors;
    try {
        Product product(sku, productName, unitPrice,
            stockQuantity, reorderLevel);
    }
    catch (const std::invalid_argument&) {
        return errors;
    }
    errors += "Expected std::invalid_argument, but construction succeeded.";
    return errors;
}

std::string testConstructorAllZeroValues(std::string sku,
    std::string productName,
    std::int64_t unitPrice,
    int stockQuantity,
    int reorderLevel) {
    std::string errors;
    try {
        errors = testConstructorMatchingValues(sku, productName, unitPrice,
            stockQuantity, reorderLevel);
        return errors;
    }
    catch (const std::invalid_argument&) {
        errors += "Constructor failed to create the product with zero values";
        return errors;
    }
}

std::string testConstructorNameContainingWhitespace(std::string sku,
    std::string productName,
    std::int64_t unitPrice,
    int stockQuantity,
    int reorderLevel) {
    std::string errors;
    try {
        Product product(sku, productName, unitPrice,
            stockQuantity, reorderLevel);
        return errors;
    }
    catch (const std::invalid_argument&) {
        errors += "Constructor rejected a valid name containing spaces.";
        return errors;
    }
}