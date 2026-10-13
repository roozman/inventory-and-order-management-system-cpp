#include "product.h"
#include "inventory.h"
#include "customer.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <limits>
#include <iomanip>
#include <algorithm>
#include <array>
#include <random>

#include "customer_registry.h"

enum class MainMenuOptions {
    AddProduct = 1,
    ListProducts = 2,
    LowStockProducts = 3,
    OpenProductOperations = 4,
    RegisterCustomer = 5,
    ListCustomers = 6,
    FindCustomerById = 7,
    Exit = 8
};

enum class ProductOperationsMenuOptions {
    EditProductName = 1,
    EditProductUnitPrice = 2,
    EditProductReorderLevel = 3,
    IncreaseStock = 4,
    DecreaseStock = 5,
    ArchiveProduct = 6,
    ReturnToMainMenu = 7
};

void showMenu();
void showProductOperationsMenu(std::vector<Product>& prodList);
void menuAddProduct(std::vector<Product>& prodList);
void menuListProducts(const std::vector<Product>& prodList);

void menuEditProductName(Product& prodToEdit);
void menuEditProductUnitPrice(Product& prodToEdit);
void menuEditProductReorderLevel(Product& prodToEdit);
void menuIncreaseProductStock(Product& prodToEdit);
void menuDecreaseProductStock(Product& prodToEdit);
void displayLowStockProducts(const std::vector<const Product*>& lowStockProds);
void menuArchiveProduct(Product& prodToEdit);
void menuRegisterCustomer(std::vector<Customer>& customersList);
void menuListCustomers(const std::vector<Customer>& customersList);
void menuFindCustomerById(std::vector<Customer>& customersList);

bool readInt64(const std::string& prompt, std::int64_t& value);
bool readInt(const std::string& prompt, int& value);
int readMenuOption();

// Temporary function for testing purposes. Will be removed later.
void createTestingData(std::vector<Product>& prodList, std::vector<Customer>& customerList);

int main() {
    std::vector<Product> productsList;
    std::vector<Customer> customersList;
    createTestingData(productsList, customersList);

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
            case MainMenuOptions::LowStockProducts: {
                const auto lowStockReport = getLowStockProducts(productsList);
                displayLowStockProducts(lowStockReport);
                break;
            }
            case MainMenuOptions::OpenProductOperations:
                showProductOperationsMenu(productsList);
                break;
            case MainMenuOptions::RegisterCustomer:
                menuRegisterCustomer(customersList);
                break;
            case MainMenuOptions::ListCustomers:
                menuListCustomers(customersList);
                break;
            case MainMenuOptions::FindCustomerById:
                menuFindCustomerById(customersList);
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

void createTestingData(std::vector<Product> &prodList, std::vector<Customer> &customerList) {
    // A fixed seed makes a failed manual run reproducible.
    std::mt19937 generator{0xC0FFEEu};
    std::array<std::string, 5> productNames{
        "Notebook", "Desk Lamp", "Keyboard", "Water Bottle", "Backpack"
    };
    std::array<std::string, 5> customerNames{
        "Alice Smith", "Bob Jones", "Carol Lee", "David Kim", "Emma Brown"
    };
    std::shuffle(productNames.begin(), productNames.end(), generator);
    std::shuffle(customerNames.begin(), customerNames.end(), generator);

    std::uniform_int_distribution<int> priceDistribution{1000, 50000};
    std::uniform_int_distribution<int> stockDistribution{0, 30};
    std::uniform_int_distribution<int> reorderDistribution{0, 10};

    for (const auto& name : productNames) {
        const auto sku = generateSKU(findNextID(prodList));
        addProduct(prodList, Product{sku, name, priceDistribution(generator),
                                     stockDistribution(generator), reorderDistribution(generator)});
    }

    for (const auto& name : customerNames) {
        const auto id = generateCustomerId(findNextCustomerId(customerList));
        addCustomer(customerList, Customer{id, name});
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
    << "3. Show low stock products\n"
    << "4. Product operations\n\n"
    << "5. Register customer\n"
    << "6. List customers\n"
    << "7. Find customer by ID\n\n"
    << "8. Exit\n";
}

void showProductOperationsMenu(std::vector<Product> &prodList) {
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

    if (prodToEdit->isArchived()) {
        std::cout << "Product " << sku << " is archived and cannot be modified.\n";
        return;
    }

    while (true) {
        std::cout
            <<  std::string(30, '-') << '\n'
            << "Product Operations\n"
            << std::string(30, '-') << '\n'
            << "1. Edit product name\n"
            << "2. Edit product unit price\n"
            << "3. Edit product reorder level\n"
            << "4. Increase stock\n"
            << "5. Decrease stock\n"
            << "6. Archive product\n"
            << "7. Return to main menu\n";

        int choice = readMenuOption();

        switch (static_cast<ProductOperationsMenuOptions>(choice)) {
            case ProductOperationsMenuOptions::EditProductName:
                menuEditProductName(*prodToEdit);
                break;
            case ProductOperationsMenuOptions::EditProductUnitPrice:
                menuEditProductUnitPrice(*prodToEdit);
                break;
            case ProductOperationsMenuOptions::EditProductReorderLevel:
                menuEditProductReorderLevel(*prodToEdit);
                break;
            case ProductOperationsMenuOptions::IncreaseStock:
                menuIncreaseProductStock(*prodToEdit);
                break;
            case ProductOperationsMenuOptions::DecreaseStock:
                menuDecreaseProductStock(*prodToEdit);
                break;
            case ProductOperationsMenuOptions::ArchiveProduct:
                menuArchiveProduct(*prodToEdit);
                return;
            case ProductOperationsMenuOptions::ReturnToMainMenu:
                return;
            default:
                std::cout << "Invalid option.\n";
                break;
        }
        std::cout << "\nPress Enter to return to the product operations menu...";

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
    std::cout << std::left
              << std::setw(8)  << "SKU"
              << std::setw(25) << "Name"
              << std::setw(15) << "Unit Price"
              << std::setw(12) << "Stock"
              << std::setw(17) << "Reorder Level"
              << std::setw(15) << "Stock Status"
              << "Availability\n";

    std::cout << std::string(105, '-') << '\n';
    for (const auto& product : prodList) {
        std::cout << std::left
                  << std::setw(8)  << product.getSku()
                  << std::setw(25) << product.getProductName()
                  << std::setw(15) << product.getUnitPrice()
                  << std::setw(12) << product.getStockQuantity()
                  << std::setw(17) << product.getReorderLevel()
                  << std::setw(15)
                  << (product.isLowStock() ? "Low stock" : "In stock")
                  << (product.isArchived() ? "Archived" : "Active")
                  << '\n';
    }
    std::cout << std::string(105, '-') << "\n\n";
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

void menuIncreaseProductStock(Product &prodToEdit) {
    int amount;
    std::cout << "Current stock: " << prodToEdit.getStockQuantity() << "\n";
    if (!readInt("Enter the amount to increase: ", amount)) {
        std::cerr << "Product edit cancelled: input unavailable.\n";
        return;
    }
    try {
        prodToEdit.increaseStock(amount);
        std::cout << "Increased the stock quantity of product " << prodToEdit.getSku()
                  << " by " << amount << " to " << prodToEdit.getStockQuantity() << ".\n";
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Cannot increase stock quantity: " << e.what() << '\n';
    }
    catch (const std::overflow_error& e) {
        std::cerr << "Cannot increase stock quantity: " << e.what() << '\n';
    }
}

void menuDecreaseProductStock(Product &prodToEdit) {
    int amount;
    std::cout << "Current stock: " << prodToEdit.getStockQuantity() << "\n";
    if (!readInt("Enter the amount to decrease: ", amount)) {
        std::cerr << "Product edit cancelled: input unavailable.\n";
        return;
    }
    try {
        prodToEdit.decreaseStock(amount);
        std::cout << "Decreased the stock quantity of product " << prodToEdit.getSku()
                  << " by " << amount << " to " << prodToEdit.getStockQuantity() << ".\n";
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Cannot decrease stock quantity: " << e.what() << '\n';
    }
    catch (const std::out_of_range& e) {
        std::cerr << "Cannot decrease stock quantity: " << e.what() << '\n';
    }
}

void displayLowStockProducts(const std::vector<const Product *>& lowStockProds) {

    std::cout << "Low-stock products\n";
    std::cout << "------------------\n";
    if (lowStockProds.empty()) {
        std::cout << "No low-stock products found.\n";
        return;
    }

    for (const Product* product : lowStockProds) {
        std::cout
            << std::left
            << std::setw(6) << product->getSku()
            << " | "
            << std::setw(20) << product->getProductName()
            << " | Stock: "
            << std::right
            << std::setw(3) << product->getStockQuantity()
            << " | Reorder level: "
            << product->getReorderLevel()
            << '\n';
    }
}

void menuArchiveProduct(Product &prodToEdit) {
    try {
        prodToEdit.archive();
        std::cout << "Product " << prodToEdit.getSku() << " archived successfully.\n";
    }
    catch (const std::logic_error& error) {
        std::cerr << "Failed to archive product: " << error.what() << '\n';
    }
}

void menuRegisterCustomer(std::vector<Customer> &customersList) {
    std::string name;
    std::cout << "Enter customer name: ";

    if (!getline(std::cin, name)) {
        std::cerr << "Customer registration cancelled: input unavailable.\n";
        return;
    }

    try {
        const auto nextId = findNextCustomerId(customersList);
        const std::string customerId = generateCustomerId(nextId);

        Customer newCustomer(customerId, name);
        addCustomer(customersList, newCustomer);
        std::cout << "Customer \"" << name
            << "\" with ID: " << newCustomer.getId()
            << " successfully registered!\n";
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Cannot register customer: " << e.what() << '\n';
    }
    catch (const std::out_of_range& e) {
        std::cerr << "Cannot generate customer ID: " << e.what() << '\n';
    }
}

void menuListCustomers(const std::vector<Customer> &customersList) {
    if (customersList.empty()) {
        std::cout << "No customers registered.\n";
        return;
    }

    std::cout << std::left
              << std::setw(12) << "Customer ID"
              << std::setw(30) << "Customer Name"
              << '\n';

    std::cout << std::string(42, '-') << '\n';

    for (const auto& customer : customersList) {
        std::cout << std::left
                  << std::setw(12) << customer.getId()
                  << std::setw(30) << customer.getName()
                  << '\n';
    }

    std::cout << std::string(42, '-') << '\n';
}

void menuFindCustomerById(std::vector<Customer> &customersList) {
    if (customersList.empty()) {
        std::cout << "No customers registered.\n";
        return;
    }
    std::string id;
    std::cout << "Enter the customer ID: ";
    if (!getline(std::cin, id)) {
        std::cerr << "Input unavailable, terminating operation.\n";
        return;
    }

    if (const Customer* foundCustomer = getCustomerById(customersList, id)) {
        std::cout << std::left
                  << std::setw(12) << "Customer ID"
                  << std::setw(30) << "Customer Name"
                  << '\n';

        std::cout << std::string(42, '-') << '\n';

        std::cout << std::left
                  << std::setw(12) << foundCustomer->getId()
                  << std::setw(30) << foundCustomer->getName()
                  << '\n';
    }
    else {
        std::cout << "Customer not found.\n";
    }
}
