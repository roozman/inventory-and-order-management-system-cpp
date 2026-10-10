#include "product.h"
#include "inventory.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <limits>
#include <iomanip>

enum class MainMenuOptions {
    AddProduct = 1,
    ListProducts = 2,
    OpenEditMenu = 3,
    Exit = 4
};

enum class EditMenuOptions {
    EditProductName = 1,
    EditProductUnitPrice = 2,
    EditProductStockQuantity = 3,
    EditProductReorderLevel = 4,
    Exit = 5
};

void showMenu();
void showEditMenu(std::vector<Product>& prodList);
void menuAddProduct(std::vector<Product>& prodList);
void menuListProducts(const std::vector<Product>& prodList);

void menuEditProductName(Product& prodToEdit);
void menuEditProductUnitPrice(Product& prodToEdit);
void menuEditProductStockQuantity(Product& prodToEdit);
void menuEditProductReorderLevel(Product& prodToEdit);

bool readInt64(const std::string& prompt, std::int64_t& value);
bool readInt(const std::string& prompt, int& value);
int readMenuOption();

int main() {
    std::vector<Product> productsList;
    int choice{0};
    while (true) {
        showMenu();
        choice = readMenuOption();

        switch (static_cast<MainMenuOptions>(choice)) {
            case MainMenuOptions::AddProduct:
                menuAddProduct(productsList);
                break;
            case MainMenuOptions::ListProducts:
                menuListProducts(productsList);
                break;
            case MainMenuOptions::OpenEditMenu:
                showEditMenu(productsList);
                break;
            case MainMenuOptions::Exit:
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

int readMenuOption() {
    int choice{0};

    std::cout << "Please choose an option: ";
    if (!(std::cin >> choice)) {
        if (std::cin.eof()) {
            return 0;
        }
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        std::cout << "Invalid input.\n";
        return 0;
    }
    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );
    return choice;
}

void showMenu() {
    std::cout
    <<  std::string(40, '=') << '\n'
    << "Inventory and Order Management System\n"
    << std::string(40, '=') << '\n'
    << "1. Add product\n"
    << "2. List products\n"
    << "3. Edit a product\n"
    << "4. Exit\n";
}

void showEditMenu(std::vector<Product> &prodList) {
    std::string sku;
    std::cout << "Enter the SKU of the product to edit: ";
    if (!getline(std::cin, sku)) {
        std::cerr << "Product edit cancelled: input unavailable.\n";
        return;
    }

    Product* prodToEdit = getProductBySKU(prodList, sku);
    if (prodToEdit == nullptr) {
        std::cout << "No product found with SKU: " << sku << "\n";
        return;
    }

    while (true) {
        std::cout
            <<  std::string(30, '-') << '\n'
            << "Product Edit Menu\n"
            << std::string(30, '-') << '\n'
            << "1. Edit product name\n"
            << "2. Edit product unit price\n"
            << "3. Edit product stock quantity\n"
            << "4. Edit product reorder level\n"
            << "5. Return to main menu\n";

        int choice = readMenuOption();

        switch (static_cast<EditMenuOptions>(choice)) {
            case EditMenuOptions::EditProductName:
                menuEditProductName(*prodToEdit);
                break;
            case EditMenuOptions::EditProductUnitPrice:
                menuEditProductUnitPrice(*prodToEdit);
                break;
            case EditMenuOptions::EditProductStockQuantity:
                menuEditProductStockQuantity(*prodToEdit);
                break;
            case EditMenuOptions::EditProductReorderLevel:
                menuEditProductReorderLevel(*prodToEdit);
                break;
            case EditMenuOptions::Exit:
                return;
            default:
                std::cout << "Invalid option.\n";
                break;
        }
        std::cout << "\nPress Enter to return to the edit menu...";

        if (std::string input; !std::getline(std::cin, input)) {
            break;
        }
    }

}

bool readInt64(const std::string& prompt, std::int64_t& value)
{
    std::string input;

    while (true) {
        std::cout << prompt;

        if (!std::getline(std::cin, input)) {
            return false;
        }

        std::istringstream stream(input);
        std::int64_t parsedValue{};

        if (stream >> parsedValue) {
            stream >> std::ws;

            if (stream.eof()) {
                value = parsedValue;
                return true;
            }
        }

        std::cout << "Invalid input. Please enter an integer.\n";
    }
}

bool readInt(const std::string &prompt, int &value) {
    std::string input;

    while (true) {
        std::cout << prompt;
        if (!std::getline(std::cin, input)) {
            return false;
        }

        std::istringstream stream(input);
        int parsedValue{};

        if (stream >> parsedValue) {
            stream >> std::ws;

            if (stream.eof()) {
                value = parsedValue;
                return true;
            }
        }

        std::cout << "Invalid input. Please enter an integer.\n";
    }
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

    if (!readInt64("Enter price: ", unitPrice)) {
        std::cerr << "Product creation cancelled: input unavailable.\n";
        return;
    }
    if (!readInt("Enter quantity: ", stockQuantity)) {
        std::cerr << "Product creation cancelled: input unavailable.\n";
        return;
    }
    if (!readInt("Enter reorder level: ", reorderLevel)) {
        std::cerr << "Product creation cancelled: input unavailable.\n";
        return;
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

void menuEditProductName(Product &prodToEdit) {
    std::string productName;

    std::cout << "Enter the new name of the product to be edited: ";
    if (!getline(std::cin, productName)) {
        std::cerr << "Product edit cancelled: input unavailable.\n";
        return;
    }

    try {
        prodToEdit.setProductName(productName);
        std::cout << "Changed the name of product " << prodToEdit.getSku()
                  << " to " << prodToEdit.getProductName() << ".\n";
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Cannot edit name: " << e.what() << '\n';
    }

}

void menuEditProductUnitPrice(Product &prodToEdit) {
    std::int64_t unitPrice;

    if (!readInt64("Enter the new unit price: ", unitPrice)) {
        std::cerr << "Product edit cancelled: input unavailable.\n";
        return;
    }

    try {
        prodToEdit.setUnitPrice(unitPrice);
        std::cout << "Changed the unit price of product " << prodToEdit.getSku()
                  << " to " << prodToEdit.getUnitPrice() << ".\n";
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Cannot edit unit price: " << e.what() << '\n';
    }

}

void menuEditProductStockQuantity(Product &prodToEdit) {
    int stockQuantity;

    if (!readInt("Enter the new stock quantity: ", stockQuantity)) {
        std::cerr << "Product edit cancelled: input unavailable.\n";
        return;
    }
    try {
        prodToEdit.setStockQuantity(stockQuantity);
        std::cout << "Changed the stock quantity of product " << prodToEdit.getSku()
                  << " to " << prodToEdit.getStockQuantity() << ".\n";
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Cannot edit stock quantity: " << e.what() << '\n';
    }

}

void menuEditProductReorderLevel(Product &prodToEdit) {
    int reorderLevel;

    if (!readInt("Enter the new reorder level: ", reorderLevel)) {
        std::cerr << "Product edit cancelled: input unavailable.\n";
        return;
    }
    try {
        prodToEdit.setReorderLevel(reorderLevel);
        std::cout << "Changed the reorder level of product " << prodToEdit.getSku()
                  << " to " << prodToEdit.getReorderLevel() << ".\n";
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Cannot edit reorder level: " << e.what() << '\n';
    }

}
