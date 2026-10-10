#include "product.h"
#include "inventory.h"
#include <iostream>
#include <stdexcept>
#include <sstream>

int main() {

    std::cout << "Welcome to the store management app!" << "\n";

    std::string productName;
    std::int64_t unitPrice{0};
    int stockQuantity{0};
    int reorderLevel{0};
    std::vector<Product> productsList;

    while (true) {
        for (const auto& prod : productsList) {
            std::cout << "-----debug-----\n";
            std::cout << "SKU: " << prod.getSku()<< " Name: " << prod.getProductName() << " Unit price: " << prod.getUnitPrice() <<
                " stock Q: " << prod.getStockQuantity() << " reorder level: " << prod.getReorderLevel() << "\n";
            std::cout << "----------\n";
        }

        std::cout << "Enter product name: ";
        if (!getline(std::cin, productName)) {
            return 1;
        }

        std::cout << "Enter price: ";
        std::string input;
        while (true) {
            if (!getline(std::cin, input)) {
                return 1;
            }

            if (std::istringstream iss(input); iss >> unitPrice) {
                iss >> std::ws;

                if (iss.eof()) {
                    break;
                }
            }
            std::cout << "invalid entry, expecting an integer! Please enter the price again:\n";
        }

        std::cout << "Enter quantity: ";
        while (true) {
            if (!getline(std::cin, input)) {
                return 1;
            }

            if (std::istringstream iss(input); iss >> stockQuantity) {
                iss >> std::ws;

                if (iss.eof()) {
                    break;
                }
            }
            std::cout << "invalid entry, expecting an integer! Please enter the stock quantity again:\n";
        }

        std::cout << "Enter reorder level: ";
        while (true) {
            if (!getline(std::cin, input)) {
                return 1;
            }

            if (std::istringstream iss(input); iss >> reorderLevel) {
                iss >> std::ws;

                if (iss.eof()) {
                    break;
                }
            }
            std::cout << "invalid entry, expecting an integer! Please enter the reorder level again:\n";
        }

        try {
            const auto nextID = findNextID(productsList);
            const std::string sku = generateSKU(nextID);

            Product newProduct(sku, productName, unitPrice,
                               stockQuantity, reorderLevel);

            addProduct(productsList, newProduct);
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "Cannot add product: " << e.what() << '\n';
        }
        catch (const std::out_of_range& e) {
            std::cerr << "Cannot generate SKU: " << e.what() << '\n';
            break;
        }
    }

}