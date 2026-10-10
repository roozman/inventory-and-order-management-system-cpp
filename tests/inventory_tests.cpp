#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "../inventory.h"

std::string inventory_lookup_empty();
std::string inventory_lookup_existing();
std::string inventory_lookup_nonempty_unknown_sku();
std::string inventory_add_first_product();
std::string inventory_add_to_nonempty_vector();
std::string inventory_add_reject_duplicate();
bool verifyFields(const Product &prod1, const Product &prod2);

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

std::string inventory_lookup_empty() {
    std::string errors;
    std::vector<Product> emptyVector{};
    if (getProductBySKU(emptyVector, "001")) {
        errors += "Searching an empty inventory returned a product instead of nullptr.\n";
        return errors;
    }
    return errors;
}

std::string inventory_lookup_existing() {
    std::string errors;
    std::vector<Product> prodList;

    Product const prod1("001", "cake", 12000, 25, 0);
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

    Product const prod1("001", "cake", 12000, 25, 0);
    prodList.push_back(prod1);

    if (getProductBySKU(prodList, "000")) {
        errors += "Searching for a missing SKU returned a product instead of nullptr.\n";
        return errors;
    }
    return errors;
}

std::string inventory_add_first_product() {
    std::string errors;
    std::vector<Product> prodList;
    Product const prod1("001", "cake", 12000, 25, 0);
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
    Product const prod1("001", "cake", 12000, 25, 0);
    Product const prod2("002", "biscuit", 12000, 25, 0);
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
    Product const prod1("001", "cake", 12000, 25, 0);
    Product const prod2("001", "biscuit", 12000, 25, 0);
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
