#include "../src/product.h"
#include <iostream>
#include <string>
#include <stdexcept>
#include <exception>
#include <limits>

std::string testConstructorInvalidSKU(const std::string& sku);

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
std::string testSetProductNameValid(const std::string& newName);
std::string testSetProductNameInvalid(const std::string& invalidName);
std::string testSetUnitPriceValid(std::int64_t newPrice);
std::string testSetUnitPriceNegative();
std::string testSetReorderLevelValid(int newLevel);
std::string testSetReorderLevelNegative();
std::string testIncreaseStockValid(int initialStock, int amount, int expectedStock);
std::string testIncreaseStockInvalidArgument(int amount);
std::string testIncreaseStockOverflow();
std::string testDecreaseStockValid(int initialStock, int amount, int expectedStock);
std::string testDecreaseStockInvalidArgument(int amount);
std::string testDecreaseStockOutOfRange();
std::string testLowStockStatus(int stockQuantity, int reorderLevel, bool expected);

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Provide one test name. \n";
        return 1;
    }

    const std::string testName{argv[1]};

    std::string errors;

    if (testName == "valid") {
        errors = testConstructorMatchingValues("P001", "biscuit", 20000, 23, 5);
    }
    else if (testName == "empty_name") {
        errors = testConstructorEmptyName("P001", "",20000, 23, 5 );
    }
    else if (testName == "whitespace_name") {
        errors = testConstructorWhitespaceOnlyName("P001", "   ", 20000, 23, 5);
    }
    else if (testName == "neg_stock") {
        errors = testConstructorNegativeStock("P001", "cake", 20000, -23, 5);
    }
    else if (testName == "neg_price") {
        errors = testConstructorNegativePrice("P001", "chips", -20000, 23, 5);
    }
    else if (testName == "neg_reorder") {
        errors = testConstructorNegativeReorder("P001", "chips", 20000, 23, -5);
    }
    else if (testName == "all_zero_values") {
        errors = testConstructorAllZeroValues("P001", "chips", 0, 0, 0);
    }
    else if (testName == "name_whitespace") {
        errors = testConstructorNameContainingWhitespace("P001", "fish cracker", 10000, 0, 0);
    }
    else if (testName == "sku_empty") {
        errors = testConstructorInvalidSKU("");
    }
    else if (testName == "sku_too_short") {
        errors = testConstructorInvalidSKU("P01");
    }
    else if (testName == "sku_too_long") {
        errors = testConstructorInvalidSKU("P0001");
    }
    else if (testName == "sku_wrong_prefix") {
        errors = testConstructorInvalidSKU("X001");
    }
    else if (testName == "sku_invalid_digit") {
        errors = testConstructorInvalidSKU("P02Z");
    }
    else if (testName == "sku_sign") {
        errors = testConstructorInvalidSKU("P+01");
    }
    else if (testName == "sku_whitespace") {
        errors = testConstructorInvalidSKU("P 01");
    }
    else if (testName == "sku_lowercase") {
        errors = testConstructorInvalidSKU("P02a");
    }
    else if (testName == "sku_reserved") {
        errors = testConstructorInvalidSKU("P000");
    }
    else if (testName == "sku_maximum") {
        errors = testConstructorMatchingValues("PFFF", "biscuit", 20000, 23, 5);
    }
    else if (testName == "set_name_valid") {
        errors = testSetProductNameValid("Chocolate Cake");
    }
    else if (testName == "set_name_with_spaces") {
        errors = testSetProductNameValid("Chocolate Cake With Cream");
    }
    else if (testName == "set_name_empty") {
        errors = testSetProductNameInvalid("");
    }
    else if (testName == "set_name_whitespace") {
        errors = testSetProductNameInvalid("   \t");
    }
    else if (testName == "set_price_positive") {
        errors = testSetUnitPriceValid(2500);
    }
    else if (testName == "set_price_zero") {
        errors = testSetUnitPriceValid(0);
    }
    else if (testName == "set_price_large") {
        errors = testSetUnitPriceValid(5'000'000'000LL);
    }
    else if (testName == "set_price_negative") {
        errors = testSetUnitPriceNegative();
    }
    else if (testName == "set_reorder_positive") {
        errors = testSetReorderLevelValid(12);
    }
    else if (testName == "set_reorder_zero") {
        errors = testSetReorderLevelValid(0);
    }
    else if (testName == "set_reorder_negative") {
        errors = testSetReorderLevelNegative();
    }
    else if (testName == "increase_stock_positive") {
        errors = testIncreaseStockValid(3, 2, 5);
    }
    else if (testName == "increase_stock_zero") {
        errors = testIncreaseStockInvalidArgument(0);
    }
    else if (testName == "increase_stock_negative") {
        errors = testIncreaseStockInvalidArgument(-1);
    }
    else if (testName == "increase_stock_to_max") {
        errors = testIncreaseStockValid(std::numeric_limits<int>::max() - 1, 1,
                                        std::numeric_limits<int>::max());
    }
    else if (testName == "increase_stock_overflow") {
        errors = testIncreaseStockOverflow();
    }
    else if (testName == "decrease_stock_positive") {
        errors = testDecreaseStockValid(3, 2, 1);
    }
    else if (testName == "decrease_stock_zero") {
        errors = testDecreaseStockInvalidArgument(0);
    }
    else if (testName == "decrease_stock_negative") {
        errors = testDecreaseStockInvalidArgument(-1);
    }
    else if (testName == "decrease_stock_to_zero") {
        errors = testDecreaseStockValid(3, 3, 0);
    }
    else if (testName == "decrease_stock_excessive") {
        errors = testDecreaseStockOutOfRange();
    }
    else if (testName == "low_stock_below") {
        errors = testLowStockStatus(2, 3, true);
    }
    else if (testName == "low_stock_equal") {
        errors = testLowStockStatus(3, 3, true);
    }
    else if (testName == "low_stock_above") {
        errors = testLowStockStatus(4, 3, false);
    }
    else if (testName == "low_stock_zero_threshold") {
        errors = testLowStockStatus(0, 0, true);
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

std::string testConstructorInvalidSKU(const std::string& sku)
{
    try {
        Product product(sku, "biscuit", 20000, 23, 5);
    }
    catch (const std::invalid_argument&) {
        return {};
    }
    catch (const std::exception& error) {
        return "Unexpected exception for SKU '" + sku + "': " + error.what() + "\n";
    }
    catch (...) {
        return "Unexpected non-standard exception for SKU '" + sku + "'.\n";
    }
    return "Invalid SKU '" + sku + "' was accepted.\n";
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

std::string testSetProductNameValid(const std::string& newName)
{
    Product product("P001", "Cake", 1000, 10, 5);
    product.setProductName(newName);

    if (product.getProductName() != newName) {
        return "Product name was not updated.\n";
    }
    return {};
}

std::string testSetProductNameInvalid(const std::string& invalidName)
{
    Product product("P001", "Cake", 1000, 10, 5);

    try {
        product.setProductName(invalidName);
    }
    catch (const std::invalid_argument&) {
        if (product.getProductName() != "Cake") {
            return "Product name changed after a rejected update.\n";
        }
        return {};
    }

    return "Expected std::invalid_argument for an invalid product name.\n";
}

std::string testSetUnitPriceValid(const std::int64_t newPrice)
{
    Product product("P001", "Cake", 1000, 10, 5);
    product.setUnitPrice(newPrice);

    if (product.getUnitPrice() != newPrice) {
        return "Unit price was not updated.\n";
    }
    return {};
}

std::string testSetUnitPriceNegative()
{
    Product product("P001", "Cake", 1000, 10, 5);

    try {
        product.setUnitPrice(-1);
    }
    catch (const std::invalid_argument&) {
        if (product.getUnitPrice() != 1000) {
            return "Unit price changed after a rejected update.\n";
        }
        return {};
    }

    return "Expected std::invalid_argument for a negative unit price.\n";
}

std::string testSetReorderLevelValid(const int newLevel)
{
    Product product("P001", "Cake", 1000, 10, 5);
    product.setReorderLevel(newLevel);

    if (product.getReorderLevel() != newLevel) {
        return "Reorder level was not updated.\n";
    }
    return {};
}

std::string testSetReorderLevelNegative()
{
    Product product("P001", "Cake", 1000, 10, 5);

    try {
        product.setReorderLevel(-1);
    }
    catch (const std::invalid_argument&) {
        if (product.getReorderLevel() != 5) {
            return "Reorder level changed after a rejected update.\n";
        }
        return {};
    }

    return "Expected std::invalid_argument for a negative reorder level.\n";
}

std::string testIncreaseStockValid(const int initialStock, const int amount,
                                   const int expectedStock)
{
    Product product("P001", "Cake", 1000, initialStock, 5);
    product.increaseStock(amount);

    if (product.getStockQuantity() != expectedStock) {
        return "Stock quantity was incorrect after an increase.\n";
    }
    return {};
}

std::string testIncreaseStockInvalidArgument(const int amount)
{
    constexpr int initialStock{3};
    Product product("P001", "Cake", 1000, initialStock, 5);

    try {
        product.increaseStock(amount);
    }
    catch (const std::invalid_argument&) {
        if (product.getStockQuantity() != initialStock) {
            return "Stock changed after a rejected increase.\n";
        }
        return {};
    }
    catch (const std::exception& error) {
        return "Unexpected exception for an invalid stock increase: "
            + std::string{error.what()} + "\n";
    }

    return "Expected std::invalid_argument for a non-positive stock increase.\n";
}

std::string testIncreaseStockOverflow()
{
    constexpr int initialStock{std::numeric_limits<int>::max()};
    Product product("P001", "Cake", 1000, initialStock, 5);

    try {
        product.increaseStock(1);
    }
    catch (const std::overflow_error&) {
        if (product.getStockQuantity() != initialStock) {
            return "Stock changed after a rejected overflowing increase.\n";
        }
        return {};
    }
    catch (const std::exception& error) {
        return "Unexpected exception for an overflowing stock increase: "
            + std::string{error.what()} + "\n";
    }

    return "Expected std::overflow_error for a stock increase overflow.\n";
}

std::string testDecreaseStockValid(const int initialStock, const int amount,
                                   const int expectedStock)
{
    Product product("P001", "Cake", 1000, initialStock, 5);
    product.decreaseStock(amount);

    if (product.getStockQuantity() != expectedStock) {
        return "Stock quantity was incorrect after a decrease.\n";
    }
    return {};
}

std::string testDecreaseStockInvalidArgument(const int amount)
{
    constexpr int initialStock{3};
    Product product("P001", "Cake", 1000, initialStock, 5);

    try {
        product.decreaseStock(amount);
    }
    catch (const std::invalid_argument&) {
        if (product.getStockQuantity() != initialStock) {
            return "Stock changed after a rejected decrease.\n";
        }
        return {};
    }
    catch (const std::exception& error) {
        return "Unexpected exception for an invalid stock decrease: "
            + std::string{error.what()} + "\n";
    }

    return "Expected std::invalid_argument for a non-positive stock decrease.\n";
}

std::string testDecreaseStockOutOfRange()
{
    constexpr int initialStock{3};
    Product product("P001", "Cake", 1000, initialStock, 5);

    try {
        product.decreaseStock(4);
    }
    catch (const std::out_of_range&) {
        if (product.getStockQuantity() != initialStock) {
            return "Stock changed after a rejected excessive decrease.\n";
        }
        return {};
    }
    catch (const std::exception& error) {
        return "Unexpected exception for an excessive stock decrease: "
            + std::string{error.what()} + "\n";
    }

    return "Expected std::out_of_range for an excessive stock decrease.\n";
}

std::string testLowStockStatus(const int stockQuantity, const int reorderLevel,
                               const bool expected)
{
    const Product product("P001", "Cake", 1000, stockQuantity, reorderLevel);

    if (product.isLowStock() != expected) {
        return "Low-stock status did not match the stock and reorder level.\n";
    }
    return {};
}
