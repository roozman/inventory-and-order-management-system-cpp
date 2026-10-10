#include "product.h"
#include "inventory.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <limits>
#include <iomanip>

enum class MenuOptions {
    AddProduct = 1,
    ListProducts = 2,
    Exit = 3
};

void showMenu();
void menuAddProduct(std::vector<Product>& prodList);
void menuListProducts(const std::vector<Product>& prodList);

int main() {
    std::vector<Product> productsList;
    int choice{0};

    while (true) {
        showMenu();
        std::cout << "Please choose an option: ";
        if (!(std::cin >> choice)) {
            if (std::cin.eof()) {
                break;
            }
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(),
                '\n'
            );

            std::cout << "Invalid input.\n";
            continue;
        }
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        switch (static_cast<MenuOptions>(choice)) {
            case MenuOptions::AddProduct:
                menuAddProduct(productsList);
                break;
            case MenuOptions::ListProducts:
                menuListProducts(productsList);
                break;
            case MenuOptions::Exit:
                return 0;
            default:
                std::cout << "Invalid option.\n";
                break;
        }
        std::cout << "\nPress Enter to return to the main menu...";

        if (std::string input; !std::getline(std::cin, input)) {
            break;
        }
    }
}

void showMenu() {
    std::cout
    <<  std::string(40, '=') << '\n'
    << "Inventory and Order Management System\n"
    << std::string(40, '=') << '\n'
    << "1. Add product\n"
    << "2. List product\n"
    << "3. Exit\n\n";
}

void menuAddProduct(std::vector<Product> &prodList) {
    std::string productName;
    std::int64_t unitPrice{0};
    int stockQuantity{0};
    int reorderLevel{0};

    std::cout << "Enter product name: ";
        if (!getline(std::cin, productName)) {
            std::cerr << "Product creation cancelled: input unavailable.\n";
            return;
        }

        std::cout << "Enter price: ";
        std::string input;
        while (true) {
            if (!getline(std::cin, input)) {
                std::cerr << "Product creation cancelled: input unavailable.\n";
                return;
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
                std::cerr << "Product creation cancelled: input unavailable.\n";
                return;
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
                std::cerr << "Product creation cancelled: input unavailable.\n";
                return;
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
            const auto nextID = findNextID(prodList);
            const std::string sku = generateSKU(nextID);

            Product newProduct(sku, productName, unitPrice,
                               stockQuantity, reorderLevel);

            addProduct(prodList, newProduct);
            std::cout << "Product " << productName
              << " with SKU: " << sku
              << " was added successfully.\n";
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "Cannot add product: " << e.what() << '\n';
        }
        catch (const std::out_of_range& e) {
            std::cerr << "Cannot generate SKU: " << e.what() << '\n';
        }
}

void menuListProducts(const std::vector<Product> &prodList) {
    if (prodList.empty()) {
        std::cout << "No products registered.\n";
        return;
    }
    std::cout
        << std::left
        << std::setw(10) << "SKU"
        << std::setw(25) << "Name"
        << std::right
        << std::setw(15) << "Unit Price"
        << std::setw(10) << "Stock"
        << std::setw(15) << "Reorder Level"
        << '\n';

    std::cout << std::string(75, '-') << '\n';
    for (const auto& product : prodList) {
        std::cout
            << std::left
            << std::setw(10) << product.getSku()
            << std::setw(25) << product.getProductName()
            << std::right
            << std::setw(15) << product.getUnitPrice()
            << std::setw(10) << product.getStockQuantity()
            << std::setw(15) << product.getReorderLevel()
            << '\n';
    }
    std::cout << std::string(75, '-') << "\n\n";
}
