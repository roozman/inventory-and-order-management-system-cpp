#include "product.h"
#include "inventory.h"
#include "customer.h"
#include "order.h"
#include "order_registry.h"
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
    OpenOrderMenu = 8,
    Exit = 9
};

enum class ProductOperationsMenuOptions {
    EditProductName = 1,
    EditProductUnitPrice = 2,
    EditProductReorderLevel = 3,
    IncreaseStock = 4,
    DecreaseStock = 5,
    SetDiscount = 6,
    ArchiveProduct = 7,
    ReturnToMainMenu = 8
};

enum class OrdersOperationsMenuOptions {
    CreateDraftOrder = 1,
    ShowOrder = 2,
    AddItem = 3,
    RemoveItem = 4,
    ConfirmOrder = 5,
    CancelOrder = 6,
    CompleteOrder = 7,
    ListOrders = 8,
    CustomerHistory = 9,
    SalesSummary = 10,
    ReturnToMainMenu = 11
};

void showMenu();
void showProductOperationsMenu(std::vector<Product>& prodList);
void showOrderOperationsMenu(std::vector<Order>& orders, std::vector<Customer>& customers, std::vector<Product>& products);

void menuAddProduct(std::vector<Product>& prodList);
void menuListProducts(const std::vector<Product>& prodList);

void menuEditProductName(Product& prodToEdit);
void menuEditProductUnitPrice(Product& prodToEdit);
void menuEditProductReorderLevel(Product& prodToEdit);
void menuIncreaseProductStock(Product& prodToEdit);
void menuDecreaseProductStock(Product& prodToEdit);
void menuSetProductDiscount(Product& prodToEdit);
void displayLowStockProducts(const std::vector<const Product*>& lowStockProds);
void menuArchiveProduct(Product& prodToEdit);
void menuRegisterCustomer(std::vector<Customer>& customersList);
void menuListCustomers(const std::vector<Customer>& customersList);
void menuFindCustomerById(std::vector<Customer>& customersList);
void displayOrder(const Order& order);
void ordersMenuCreateDraftOrder(std::vector<Order>& orders, std::vector<Customer>& customers);
void ordersMenuShowOrder(std::vector<Order>& ordersList);
void ordersMenuAddItem(std::vector<Order>& orders, std::vector<Product>& products);
void ordersMenuRemoveItem(std::vector<Order>& orders);
void ordersMenuConfirmOrder(std::vector<Order>& orders, std::vector<Product>& products);
void ordersMenuCancelOrder(std::vector<Order>& orders, std::vector<Product>& products);
void ordersMenuCompleteOrder(std::vector<Order>& orders);
void ordersMenuListOrders(const std::vector<Order>& orders);
void ordersMenuCustomerHistory(const std::vector<Order>& orders, std::vector<Customer>& customers);
void ordersMenuSalesSummary(const std::vector<Order>& orders);
void displayOrders(const std::vector<const Order*>& orders);
std::string getOrderStatusName(OrderStatus status);

bool readInt64(const std::string& prompt, std::int64_t& value);
bool readInt(const std::string& prompt, int& value);
int readMenuOption();

// Temporary function for testing purposes. Will be removed later.
void createTestingData(std::vector<Product>& prodList, std::vector<Customer>& customerList,
                       std::vector<Order>& ordersList);

int main() {
    std::vector<Product> productsList;
    std::vector<Customer> customersList;
    std::vector<Order> ordersList;
    createTestingData(productsList, customersList, ordersList);

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
            case MainMenuOptions::OpenOrderMenu:
                showOrderOperationsMenu(ordersList, customersList, productsList);
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

void createTestingData(std::vector<Product> &prodList, std::vector<Customer> &customerList,
                       std::vector<Order>& ordersList) {
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

    // Draft items capture current product prices without changing stock.
    addDraftOrder(customerList, ordersList, customerList.at(0).getId());
    const auto firstOrderId = ordersList.back().getId();
    addItemToOrder(ordersList, prodList, firstOrderId, prodList.at(0).getSku(), 2);
    addItemToOrder(ordersList, prodList, firstOrderId, prodList.at(1).getSku(), 3);

    addDraftOrder(customerList, ordersList, customerList.at(1).getId());
    const auto secondOrderId = ordersList.back().getId();
    addItemToOrder(ordersList, prodList, secondOrderId, prodList.at(2).getSku(), 1);
    addItemToOrder(ordersList, prodList, secondOrderId, prodList.at(4).getSku(), 2);
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
    << "8. Show order menu\n"
    << "9. Exit\n";
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
            << "6. Edit product discount\n"
            << "7. Archive product\n"
            << "8. Return to main menu\n";

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
            case ProductOperationsMenuOptions::SetDiscount:
                menuSetProductDiscount(*prodToEdit);
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
              << std::setw(22) << "Unit Price (rials)"
              << std::setw(14) << "Discount (%)"
              << std::setw(12) << "Stock"
              << std::setw(17) << "Reorder Level"
              << std::setw(15) << "Stock Status"
              << "Availability\n";

    std::cout << std::string(126, '-') << '\n';
    for (const auto& product : prodList) {
        std::cout << std::left
                  << std::setw(8)  << product.getSku()
                  << std::setw(25) << product.getProductName()
                  << std::setw(22) << product.getUnitPrice()
                  << std::setw(14) << product.getDiscount()
                  << std::setw(12) << product.getStockQuantity()
                  << std::setw(17) << product.getReorderLevel()
                  << std::setw(15)
                  << (product.isLowStock() ? "Low stock" : "In stock")
                  << (product.isArchived() ? "Archived" : "Active")
                  << '\n';
    }
    std::cout << std::string(126, '-') << "\n\n";
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

void displayOrder(const Order& order) {
    // Calculate before printing so overflow does not leave a partial report.
    const auto subtotal = order.getSubTotal();
    const auto discount = order.getDiscountAmount();
    const auto finalTotal = order.getFinalTotal();

    std::cout << std::string(146, '=') << '\n'
              << "Order ID: " << order.getId() << '\n'
              << "Customer ID: " << order.getCustomerId() << '\n';

    std::cout << "Status: " << getOrderStatusName(order.getOrderStatus());
    std::cout << "\nAmounts in rials.\n" << std::string(146, '-') << '\n';

    if (order.getItems().empty()) {
        std::cout << "No items in this order.\n";
    }
    else {
        std::cout << std::left
                  << std::setw(8) << "SKU"
                  << std::setw(25) << "Name"
                  << std::setw(22) << "Unit Price"
                  << std::setw(11) << "Quantity"
                  << std::setw(22) << "Line Subtotal"
                  << std::setw(14) << "Discount (%)"
                  << std::setw(22) << "Discount Amount"
                  << std::setw(22) << "Line Final Total" << '\n';
        std::cout << std::string(146, '-') << '\n';

        for (const auto& item : order.getItems()) {
            std::cout << std::left
                      << std::setw(8) << item.getProductSKU()
                      << std::setw(25) << item.getProductName()
                      << std::setw(22) << item.getProductUnitPrice()
                      << std::setw(11) << item.getOrderQuantity()
                      << std::setw(22) << item.getTotalPrice()
                      << std::setw(14) << item.getDiscount()
                      << std::setw(22) << item.getDiscountAmount()
                      << std::setw(22) << item.getFinalTotal() << '\n';
        }
    }

    std::cout << std::string(146, '-') << '\n'
              << std::left << std::setw(22) << "Subtotal:" << subtotal << '\n'
              << std::setw(22) << "Discount:" << discount << '\n'
              << std::setw(22) << "Final total:" << finalTotal << '\n'
              << std::string(146, '=') << "\n\n";
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

void ordersMenuShowOrder(std::vector<Order> &ordersList) {
    if (ordersList.empty()) {
        std::cout << "No orders registered.\n";
        return;
    }

    std::string orderId;
    std::cout << "Enter the order ID: ";
    if (!std::getline(std::cin, orderId)) {
        std::cerr << "Order display cancelled: input unavailable.\n";
        return;
    }

    const Order* foundOrder = getOrderById(ordersList, orderId);
    if (foundOrder == nullptr) {
        std::cout << "No order found with ID: " << orderId << '\n';
        return;
    }

    try {
        displayOrder(*foundOrder);
    }
    catch (const std::overflow_error& e) {
        std::cerr << "Cannot display order totals: " << e.what() << '\n';
    }
}

void showOrderOperationsMenu(std::vector<Order> &ordersList, std::vector<Customer> &customers, std::vector<Product> &products) {

    while (true) {
        std::cout
            <<  std::string(30, '-') << '\n'
            << "Orders Operations\n"
            << std::string(30, '-') << '\n'
            << "1. Create Draft order\n"
            << "2. Show order\n"
            << "3. Add item\n"
            << "4. Remove Item\n"
            << "5. Confirm order\n"
            << "6. Cancel order\n"
            << "7. Complete order\n"
            << "8. List all orders\n"
            << "9. Customer order history\n"
            << "10. Sales summary\n"
            << "11. Return to main menu\n";

        int choice = readMenuOption();

        switch (static_cast<OrdersOperationsMenuOptions>(choice)) {
            case OrdersOperationsMenuOptions::CreateDraftOrder:
                ordersMenuCreateDraftOrder(ordersList, customers);
                break;
            case OrdersOperationsMenuOptions::ShowOrder:
                ordersMenuShowOrder(ordersList);
                break;
            case OrdersOperationsMenuOptions::AddItem:
                ordersMenuAddItem(ordersList, products);
                break;
            case OrdersOperationsMenuOptions::RemoveItem:
                ordersMenuRemoveItem(ordersList);
                break;
            case OrdersOperationsMenuOptions::ConfirmOrder:
                ordersMenuConfirmOrder(ordersList, products);
                break;
            case OrdersOperationsMenuOptions::CancelOrder:
                ordersMenuCancelOrder(ordersList, products);
                break;
            case OrdersOperationsMenuOptions::CompleteOrder:
                ordersMenuCompleteOrder(ordersList);
                break;
            case OrdersOperationsMenuOptions::ListOrders:
                ordersMenuListOrders(ordersList);
                break;
            case OrdersOperationsMenuOptions::CustomerHistory:
                ordersMenuCustomerHistory(ordersList, customers);
                break;
            case OrdersOperationsMenuOptions::SalesSummary:
                ordersMenuSalesSummary(ordersList);
                break;
            case OrdersOperationsMenuOptions::ReturnToMainMenu:
                return;
            default:
                std::cout << "Invalid option.\n";
                break;
        }
        std::cout << "\nPress Enter to return to the order operations menu...";

        if (std::string input; !std::getline(std::cin, input)) {
            break;
        }
    }

}

void ordersMenuCreateDraftOrder(std::vector<Order> &orders, std::vector<Customer> &customers) {
    if (customers.empty()) {
        std::cout << "No customers registered. Register a customer before creating an order.\n";
        return;
    }

    std::string customerId;
    std::cout << "Enter the customer ID: ";
    if (!std::getline(std::cin, customerId)) {
        std::cerr << "Order creation cancelled: input unavailable.\n";
        return;
    }

    try {
        addDraftOrder(customers, orders, customerId);
        std::cout << "Draft order " << orders.back().getId()
                  << " created successfully for customer " << customerId << ".\n";
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Cannot create order: " << e.what() << '\n';
    }
    catch (const std::out_of_range& e) {
        std::cerr << "Cannot generate order ID: " << e.what() << '\n';
    }
}

void ordersMenuConfirmOrder(std::vector<Order> &orders, std::vector<Product> &products) {
    if (orders.empty()) {
        std::cout << "There are no orders to confirm.\n";
        return;
    }

    std::string orderId;
    std::cout << "Enter Order ID: ";
    if (!std::getline(std::cin, orderId)) {
        std::cerr << "Order confirmation cancelled: input unavailable.\n";
        return;
    }

    try {
        confirmOrder(orders, products, orderId);
        std::cout << "Order " << orderId << " confirmed successfully.\n";
    }
    catch (const std::logic_error &e) {
        std::cerr << "Cannot confirm order: " << e.what() << "\n";
    }
    catch (const std::overflow_error &e) {
        std::cerr << "Cannot confirm order: " << e.what() << "\n";
    }
}

void ordersMenuCancelOrder(std::vector<Order> &orders, std::vector<Product> &products) {
    if (orders.empty()) {
        std::cout << "There are no orders to cancel.\n";
        return;
    }

    std::string orderId;
    std::cout << "Enter Order ID: ";
    if (!std::getline(std::cin, orderId)) {
        std::cerr << "Order cancellation stopped: input unavailable.\n";
        return;
    }

    const Order* order = getOrderById(orders, orderId);
    if (!order) {
        std::cout << "No order found with ID: " << orderId << "\n";
        return;
    }

    try {
        // Each operation enforces its own status rules in the core.
        if (order->getOrderStatus() == OrderStatus::Draft) {
            cancelDraftOrder(orders, orderId);
        }
        else {
            cancelConfirmedOrder(orders, orderId, products);
        }
        std::cout << "Order " << orderId << " cancelled successfully.\n";
    }
    catch (const std::logic_error &e) {
        std::cerr << "Cannot cancel order: " << e.what() << "\n";
    }
    catch (const std::overflow_error &e) {
        std::cerr << "Cannot cancel order: " << e.what() << "\n";
    }
}

void ordersMenuCompleteOrder(std::vector<Order> &orders) {
    if (orders.empty()) {
        std::cout << "There are no orders to complete.\n";
        return;
    }

    std::string orderId;
    std::cout << "Enter Order ID: ";
    if (!std::getline(std::cin, orderId)) {
        std::cerr << "Order completion cancelled: input unavailable.\n";
        return;
    }

    try {
        completeOrder(orders, orderId);
        std::cout << "Order " << orderId << " completed successfully.\n";
    }
    catch (const std::logic_error &e) {
        std::cerr << "Cannot complete order: " << e.what() << "\n";
    }
}

void ordersMenuRemoveItem(std::vector<Order> &orders) {
    if (orders.empty()) {
        std::cout << "There are no orders to remove an item from.\n";
        return;
    }

    std::string orderId;
    std::cout << "Enter Order ID: ";
    if (!std::getline(std::cin, orderId)) {
        std::cerr << "Item removal cancelled: input unavailable.\n";
        return;
    }

    std::string sku;
    std::cout << "Enter product SKU to remove from order: ";
    if (!std::getline(std::cin, sku)) {
        std::cerr << "Item removal cancelled: input unavailable.\n";
        return;
    }

    try {
        removeItemFromOrder(orders, orderId, sku);
        std::cout << "Item " << sku << " removed from order " << orderId << "\n";
    }
    catch (const std::invalid_argument &e) {
        std::cerr << "Cannot remove item from order: " << e.what() << "\n";
    }
    catch (const std::logic_error &e) {
        std::cerr << "Cannot remove item from order: " << e.what() << "\n";
    }
}

void ordersMenuAddItem(std::vector<Order> &orders, std::vector<Product> &products) {
    if (orders.empty()) {
        std::cout << "There are no orders to add item to.\n";
        return;
    }

    std::string orderId;
    std::cout << "Enter Order ID: ";
    if (!std::getline(std::cin, orderId)) {
        std::cerr << "Item addition failed: input unavailable.\n";
        return;
    }

    //TODO - validate sku input
    std::string sku;
    std::cout << "Enter product SKU to add to order: ";
    if (!std::getline(std::cin, sku)) {
        std::cerr << "Item addition failed: input unavailable.\n";
        return;

    }

    int quantity{0};
    if (!readInt("Enter product quantity to add to order: ", quantity)) {
        std::cerr << "Item addition cancelled: input unavailable.\n";
        return;
    }

    try {
        addItemToOrder(orders, products, orderId, sku, quantity);
        std::cout << quantity << " of item " << sku << " added to order " << orderId << "\n";
    }
    catch (const std::invalid_argument &e) {
        std::cerr << "Cannot add item to order: " << e.what() << "\n";
    }
    catch (const std::logic_error &e) {
        std::cerr << "Cannot add item to order: " << e.what() << "\n";
    }
    catch (const std::overflow_error &e) {
        std::cerr << "Cannot add item to order: " << e.what() << "\n";
    }
}

std::string getOrderStatusName(OrderStatus status) {
    switch (status) {
        case OrderStatus::Draft: return "Draft";
        case OrderStatus::Confirmed: return "Confirmed";
        case OrderStatus::Completed: return "Completed";
        case OrderStatus::Cancelled: return "Cancelled";
    }
    return "Unknown";
}

void displayOrders(const std::vector<const Order*>& orders) {
    if (orders.empty()) {
        std::cout << "No orders found.\n";
        return;
    }

    // Prepare the report before printing so overflow cannot leave a partial table.
    std::ostringstream report;
    report << "Amounts in rials.\n"
           << std::left
           << std::setw(12) << "Order ID"
           << std::setw(14) << "Customer ID"
           << std::setw(14) << "Status"
           << std::setw(22) << "Subtotal"
           << std::setw(22) << "Discount"
           << std::setw(22) << "Final Total" << '\n'
           << std::string(106, '-') << '\n';
    for (const auto* order : orders) {
        report << std::left
               << std::setw(12) << order->getId()
               << std::setw(14) << order->getCustomerId()
               << std::setw(14) << getOrderStatusName(order->getOrderStatus())
               << std::setw(22) << order->getSubTotal()
               << std::setw(22) << order->getDiscountAmount()
               << std::setw(22) << order->getFinalTotal() << '\n';
    }
    report << std::string(106, '-') << "\n\n";
    std::cout << report.str();
}

void ordersMenuListOrders(const std::vector<Order>& orders) {
    std::vector<const Order*> ordersToDisplay;
    for (const auto& order : orders) {
        ordersToDisplay.push_back(&order);
    }
    try {
        displayOrders(ordersToDisplay);
    }
    catch (const std::overflow_error& e) {
        std::cerr << "Cannot display orders: " << e.what() << '\n';
    }
}

void ordersMenuCustomerHistory(const std::vector<Order>& orders, std::vector<Customer>& customers) {
    std::string customerId;
    std::cout << "Enter the customer ID: ";
    if (!std::getline(std::cin, customerId)) {
        std::cerr << "Customer history cancelled: input unavailable.\n";
        return;
    }
    const Customer* customer = getCustomerById(customers, customerId);
    if (!customer) {
        std::cout << "No customer found with ID: " << customerId << '\n';
        return;
    }
    const auto customerOrders = getCustomerOrders(orders, customerId);
    if (customerOrders.empty()) {
        std::cout << "No orders found for customer " << customerId << ".\n";
        return;
    }
    try {
        displayOrders(customerOrders);
    }
    catch (const std::overflow_error& e) {
        std::cerr << "Cannot display customer history: " << e.what() << '\n';
    }
}

void ordersMenuSalesSummary(const std::vector<Order>& orders) {
    try {
        const auto summary = getSalesSummary(orders);
        std::cout << std::string(45, '=') << '\n'
                  << "Sales Summary (rials)\n"
                  << std::string(45, '-') << '\n'
                  << std::left << std::setw(22) << "Completed orders:" << summary.completedOrders << '\n'
                  << std::setw(22) << "Subtotal:" << summary.subtotal << '\n'
                  << std::setw(22) << "Discount:" << summary.discount << '\n'
                  << std::setw(22) << "Final revenue:" << summary.revenue << '\n';
        if (summary.completedOrders == 0) {
            std::cout << "No completed sales yet.\n";
        }
        std::cout << std::string(45, '=') << "\n\n";
    }
    catch (const std::overflow_error& e) {
        std::cerr << "Cannot display sales summary: " << e.what() << '\n';
    }
}

void menuSetProductDiscount(Product &prodToEdit) {
    int discount{0};
    if (!readInt("Please enter a discount percentage ranging from 0 to 100: ", discount)) {
        std::cerr << "Discount update cancelled: input unavailable.\n";
        return;
    }

    try {
        prodToEdit.setDiscount(discount);
        std::cout << "Discount of product " << prodToEdit.getProductName() << " set to " << prodToEdit.getDiscount()
            << "%\n";
    }
    catch (const std::invalid_argument &e) {
        std::cerr << "Cannot set discount " << e.what() << "\n";
    }
}
