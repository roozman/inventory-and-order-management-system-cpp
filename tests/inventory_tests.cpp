#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "../src/inventory.h"

std::string inventory_lookup_empty();
std::string inventory_lookup_existing();
std::string inventory_lookup_nonempty_unknown_sku();
std::string inventory_add_first_product();
std::string inventory_add_to_nonempty_vector();
std::string inventory_add_reject_duplicate();
bool verifyFields(const Product &prod1, const Product &prod2);
std::string sku_formatting();
std::string sku_reject_zero();
std::string sku_reject_overflow();
std::string sku_next_empty();
std::string sku_next_unordered();
std::string sku_next_exhausted();
std::string sku_consecutive_registration();
std::string low_stock_empty_inventory();
std::string low_stock_no_matches();
std::string low_stock_one_match();
std::string low_stock_mixed_inventory();
std::string low_stock_zero_threshold();
std::string archived_product_remains_findable();
std::string archived_sku_remains_reserved();
std::string archived_product_excluded_from_low_stock();
std::string archiving_one_product_does_not_affect_another();

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Provide one test name.\n";
        return 1;
    }

    const std::string testName{argv[1]};
    std::string errors;

    if (testName == "inventory_lookup_empty") {
        errors = inventory_lookup_empty();
    }
    else if (testName == "inventory_lookup_existing") {
        errors = inventory_lookup_existing();
    }
    else if (testName == "inventory_lookup_nonempty_unknown_sku") {
        errors = inventory_lookup_nonempty_unknown_sku();
    }
    else if (testName == "inventory_add_first_product") {
        errors = inventory_add_first_product();
    }
    else if (testName == "inventory_add_to_nonempty_vector") {
        errors = inventory_add_to_nonempty_vector();
    }
    else if (testName == "inventory_add_reject_duplicate") {
        errors = inventory_add_reject_duplicate();
    }
    else if (testName == "sku_formatting") {
        errors = sku_formatting();
    }
    else if (testName == "sku_reject_zero") {
        errors = sku_reject_zero();
    }
    else if (testName == "sku_reject_overflow") {
        errors = sku_reject_overflow();
    }
    else if (testName == "sku_next_empty") {
        errors = sku_next_empty();
    }
    else if (testName == "sku_next_unordered") {
        errors = sku_next_unordered();
    }
    else if (testName == "sku_next_exhausted") {
        errors = sku_next_exhausted();
    }
    else if (testName == "sku_consecutive_registration") {
        errors = sku_consecutive_registration();
    }
    else if (testName == "low_stock_empty_inventory") {
        errors = low_stock_empty_inventory();
    }
    else if (testName == "low_stock_no_matches") {
        errors = low_stock_no_matches();
    }
    else if (testName == "low_stock_one_match") {
        errors = low_stock_one_match();
    }
    else if (testName == "low_stock_mixed_inventory") {
        errors = low_stock_mixed_inventory();
    }
    else if (testName == "low_stock_zero_threshold") {
        errors = low_stock_zero_threshold();
    }
    else if (testName == "archived_product_remains_findable") {
        errors = archived_product_remains_findable();
    }
    else if (testName == "archived_sku_remains_reserved") {
        errors = archived_sku_remains_reserved();
    }
    else if (testName == "archived_product_excluded_from_low_stock") {
        errors = archived_product_excluded_from_low_stock();
    }
    else if (testName == "archiving_one_product_does_not_affect_another") {
        errors = archiving_one_product_does_not_affect_another();
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

bool verifyFields(const Product &prod1, const Product &prod2) {
    if (prod1.getSku() != prod2.getSku()
        || prod1.getProductName() != prod2.getProductName()
        || prod1.getUnitPrice() != prod2.getUnitPrice()
        || prod1.getStockQuantity() != prod2.getStockQuantity()
        || prod1.getReorderLevel() != prod2.getReorderLevel()) {
        return false;
    }
    return true;
}

std::string sku_formatting() {
    std::string errors;
    if (generateSKU(1) != "P001") {
        errors += "ID 1 should be formatted as P001.\n";
    }
    if (generateSKU(42) != "P02A") {
        errors += "ID 42 should be formatted as P02A.\n";
    }
    if (generateSKU(4095) != "PFFF") {
        errors += "ID 4095 should be formatted as PFFF.\n";
    }
    return errors;
}

std::string sku_reject_zero() {
    try {
        generateSKU(0);
    }
    catch (const std::out_of_range&) {
        return {};
    }
    return "Reserved ID zero was accepted.\n";
}

std::string sku_reject_overflow() {
    try {
        generateSKU(4096);
    }
    catch (const std::out_of_range&) {
        return {};
    }
    return "An ID above 0xFFF was accepted.\n";
}

std::string sku_next_empty() {
    const std::vector<Product> products;
    if (findNextID(products) != 1) {
        return "An empty inventory should start at ID 1.\n";
    }
    return {};
}

std::string sku_next_unordered() {
    const std::vector<Product> products{
        Product("P00A", "Cake", 12000, 25, 1),
        Product("P002", "Biscuit", 30000, 10, 2)
    };
    if (findNextID(products) != 11) {
        return "The next ID should be 11, even when the largest SKU is not last.\n";
    }
    return {};
}

std::string sku_next_exhausted() {
    const std::vector<Product> products{
        Product("PFFF", "Cake", 12000, 25, 1),
        Product("P001", "Biscuit", 30000, 10, 2)
    };
    try {
        findNextID(products);
    }
    catch (const std::out_of_range&) {
        return {};
    }
    return "An inventory containing PFFF should report ID exhaustion.\n";
}

std::string sku_consecutive_registration() {
    std::vector<Product> products;
    const auto firstSKU = generateSKU(findNextID(products));
    addProduct(products, Product(firstSKU, "Cake", 12000, 25, 1));

    const auto secondSKU = generateSKU(findNextID(products));
    addProduct(products, Product(secondSKU, "Biscuit", 30000, 10, 2));

    if (products.size() != 2 || products.at(0).getSku() != "P001"
        || products.at(1).getSku() != "P002") {
        return "Consecutive registrations should store P001 and P002.\n";
    }
    return {};
}

std::string inventory_lookup_empty() {
    std::string errors;
    std::vector<Product> emptyVector{};
    if (getProductBySKU(emptyVector, "P001")) {
        errors += "Searching an empty inventory returned a product instead of nullptr.\n";
        return errors;
    }
    return errors;
}

std::string inventory_lookup_existing() {
    std::string errors;
    std::vector<Product> prodList;

    Product const prod1("P001", "cake", 12000, 25, 0);
    prodList.push_back(prod1);

    if (const auto* storedProd = getProductBySKU(prodList, prod1.getSku())) {
        if (!verifyFields(*storedProd, prod1)) {
            errors += "The found product fields do not match the expected values.\n";
            return errors;
    }
        return errors;
    }
    errors += "The existing product was not found.\n";
    return errors;
}

std::string inventory_lookup_nonempty_unknown_sku() {
    std::string errors;
    std::vector<Product> prodList;

    Product const prod1("P001", "cake", 12000, 25, 0);
    prodList.push_back(prod1);

    if (getProductBySKU(prodList, "P003")) {
        errors += "Searching for a missing SKU returned a product instead of nullptr.\n";
        return errors;
    }
    return errors;
}

std::string inventory_add_first_product() {
    std::string errors;
    std::vector<Product> prodList;
    Product const prod1("P001", "cake", 12000, 25, 0);
    addProduct(prodList, prod1);
    if (prodList.empty()) {
        errors += "The product was not added to the empty inventory.\n";
        return errors;
    }
    if (prodList.size() != 1) {
        errors += "Expected inventory size 1 after insertion.\n";
        return errors;
    }

    Product tempProd = prodList.at(0);
    if (!verifyFields(tempProd, prod1)) {
        errors += "The stored product fields do not match the expected values.\n";
        return errors;
    }
    return errors;
}

std::string inventory_add_to_nonempty_vector() {
    std::string errors;
    std::vector<Product> prodList;
    Product const prod1("P001", "cake", 12000, 25, 0);
    Product const prod2("P002", "biscuit", 12000, 25, 0);
    prodList.push_back(prod1);
    addProduct(prodList, prod2);
    if (prodList.size() == 1) {
        errors += "The new product was not added to the inventory.\n";
        return errors;
    }
    if (prodList.size() != 2) {
        errors += "Expected inventory size 2 after insertion.\n";
        return errors;
    }

    Product tempProd = prodList.at(0);
    if (!verifyFields(tempProd, prod1)) {
        errors += "The existing product changed after insertion.\n";
        return errors;
        }
    const Product* addedProduct =
    getProductBySKU(prodList, prod2.getSku());

    if (addedProduct == nullptr) {
        errors += "The newly added product was not found.\n";
        return errors;
    }

    if (!verifyFields(*addedProduct, prod2)) {
        errors += "The newly added product fields do not match the expected values.\n";
        return errors;
    }
    return errors;
}

std::string inventory_add_reject_duplicate() {
    std::string errors;
    std::vector<Product> prodList;
    Product const prod1("P001", "cake", 12000, 25, 0);
    Product const prod2("P001", "biscuit", 12000, 25, 0);
    addProduct(prodList, prod1);
    try {
        addProduct(prodList, prod2);
        errors += "A duplicate SKU was accepted.\n";
        return errors;
    }
    catch (const std::invalid_argument& e) {
        if (prodList.size() != 1) {
            errors += "Rejecting the duplicate changed the inventory size.\n";
            return errors;
        }
        if (!verifyFields(prodList.at(0), prod1)) {
            errors += "Rejecting the duplicate changed the existing product.\n";
            return errors;
        }
        return errors;
    }
}

std::string low_stock_empty_inventory()
{
    const std::vector<Product> products;
    const auto result = getLowStockProducts(products);

    if (!result.empty()) {
        return "An empty inventory produced low-stock results.\n";
    }
    return {};
}

std::string low_stock_no_matches()
{
    const std::vector<Product> products{
        Product("P001", "Cake", 1000, 4, 3),
        Product("P002", "Biscuit", 2000, 10, 2)
    };
    const auto result = getLowStockProducts(products);

    if (!result.empty()) {
        return "Products above their reorder levels were reported as low stock.\n";
    }
    return {};
}

std::string low_stock_one_match()
{
    const std::vector<Product> products{
        Product("P001", "Cake", 1000, 2, 3),
        Product("P002", "Biscuit", 2000, 10, 2)
    };
    const auto result = getLowStockProducts(products);

    if (result.size() != 1 || result.front() != &products.at(0)) {
        return "The low-stock report did not return the expected product.\n";
    }
    return {};
}

std::string low_stock_mixed_inventory()
{
    const std::vector<Product> products{
        Product("P001", "Cake", 1000, 2, 3),
        Product("P002", "Biscuit", 2000, 3, 3),
        Product("P003", "Juice", 3000, 4, 3)
    };
    const auto result = getLowStockProducts(products);

    if (result.size() != 2 || result.at(0) != &products.at(0)
        || result.at(1) != &products.at(1)) {
        return "The mixed inventory low-stock results were incorrect.\n";
    }
    return {};
}

std::string low_stock_zero_threshold()
{
    const std::vector<Product> products{
        Product("P001", "Cake", 1000, 0, 0),
        Product("P002", "Biscuit", 2000, 1, 0)
    };
    const auto result = getLowStockProducts(products);

    if (result.size() != 1 || result.front() != &products.at(0)) {
        return "Zero stock with a zero reorder level was not reported as low stock.\n";
    }
    return {};
}

std::string archived_product_remains_findable()
{
    std::vector<Product> products{
        Product("P001", "Cake", 1000, 2, 3)
    };
    products.at(0).archive();

    const Product* found = getProductBySKU(products, "P001");
    if (found == nullptr || found != &products.at(0) || !found->isArchived()) {
        return "General SKU lookup did not return the archived product.\n";
    }
    return {};
}

std::string archived_sku_remains_reserved()
{
    std::vector<Product> products{
        Product("P001", "Cake", 1000, 2, 3)
    };
    products.at(0).archive();

    try {
        addProduct(products, Product("P001", "Replacement", 2000, 5, 1));
    }
    catch (const std::invalid_argument&) {
        if (products.size() != 1 || !products.at(0).isArchived()) {
            return "Rejecting reuse of an archived SKU changed the inventory.\n";
        }
        return {};
    }
    catch (const std::exception& error) {
        return "Unexpected exception for reuse of an archived SKU: "
            + std::string{error.what()} + "\n";
    }

    return "An archived SKU was reused for a new product.\n";
}

std::string archived_product_excluded_from_low_stock()
{
    std::vector<Product> products{
        Product("P001", "Archived Cake", 1000, 2, 3),
        Product("P002", "Active Biscuit", 2000, 1, 3)
    };
    products.at(0).archive();

    const auto result = getLowStockProducts(products);
    if (result.size() != 1 || result.front() != &products.at(1)) {
        return "The low-stock report did not exclude the archived product.\n";
    }
    return {};
}

std::string archiving_one_product_does_not_affect_another()
{
    std::vector<Product> products{
        Product("P001", "Cake", 1000, 2, 3),
        Product("P002", "Biscuit", 2000, 5, 1)
    };
    products.at(0).archive();

    if (!products.at(0).isArchived() || products.at(1).isArchived()) {
        return "Archiving one product affected another product's state.\n";
    }
    return {};
}
