#include "product.h"
#include <iostream>
#include <stdexcept>
#include <sstream>

int main() {
    std::cout << "Welcome to the store management app!" << "\n";

    std::string productName;
    std::int64_t unitPrice{0};
    int stockQuantity{0};
    int reorderLevel{0};

    while (true) {
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
            Product prod1("1aa", productName, unitPrice, stockQuantity,  reorderLevel);
            std::cout << "Product name: " << prod1.getProductName() << " successfully created!\n";
            std::cout << "Stock keeping unit: " << prod1.getSku() << " | Unit price: " << prod1.getUnitPrice() << "\n";
            std::cout << "Stock quantity: " << prod1.getStockQuantity() << " | Reorder level: " << prod1.getReorderLevel() << "\n";
            break;
        }
        catch (const std::invalid_argument& e) {
            std::cout << "Invalid argument: " << e.what() << "\n";
        }
    }

}