#include "../src/order.h"
#include "../src/order_item.h"
#include "../src/order_registry.h"

#include <exception>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::string testNewDraft() {
    std::vector<Customer> customers{{"C001", "Alice"}};
    std::vector<Order> orders;
    Product product{"P001", "Notebook", 20000, 5, 1};
    const int stockBefore = product.getStockQuantity();

    addDraftOrder(customers, orders, "C001");
    if (orders.size() != 1 || orders[0].getId() != "O000001" ||
        orders[0].getCustomerId() != "C001" ||
        orders[0].getOrderStatus() != OrderStatus::Draft || !orders[0].getItems().empty()) {
        return "New order did not retain its generated ID, customer, and Draft status.";
    }
    return product.getStockQuantity() == stockBefore
        ? "" : "Creating a Draft changed product stock.";
}

std::string testMissingCustomer() {
    std::vector<Customer> customers{{"C001", "Alice"}};
    std::vector<Order> orders;
    try {
        addDraftOrder(customers, orders, "C002");
    } catch (const std::invalid_argument&) {
        return orders.empty() ? "" : "Failed registration added an order.";
    }
    return "An order was created for a missing customer.";
}

std::string testUniqueIds() {
    std::vector<Customer> customers{{"C001", "Alice"}};
    std::vector<Order> orders;
    addDraftOrder(customers, orders, "C001");
    addDraftOrder(customers, orders, "C001");
    return orders.size() == 2 && orders[0].getId() == "O000001" &&
           orders[1].getId() == "O000002"
        ? "" : "Consecutive orders did not receive distinct IDs.";
}

std::string testNextIdUnordered() {
    const std::vector<Order> orders{{"O000010", "C001"}, {"O000002", "C001"}};
    return findNextOrderId(orders) == 11
        ? "" : "Next ID did not follow the largest existing order ID.";
}

std::string testIdExhausted() {
    std::vector<Customer> customers{{"C001", "Alice"}};
    std::vector<Order> orders{{"O999999", "C001"}};
    try {
        addDraftOrder(customers, orders, "C001");
    } catch (const std::out_of_range&) {
        return orders.size() == 1 ? "" : "ID exhaustion changed the order list.";
    }
    return "Registration accepted an exhausted order ID range.";
}

std::string testIdValidation() {
    for (const auto& id : {"", "O000000", "O00001", "O0000001", "P000001", "O00000A"}) {
        try {
            const Order order{id, "C001"};
        } catch (const std::invalid_argument&) {
            continue;
        }
        return "Order accepted invalid ID: " + std::string{id};
    }
    return {};
}

std::string testLookup() {
    std::vector<Order> orders;
    if (getOrderById(orders, "O000001") != nullptr) {
        return "Lookup found an order in an empty registry.";
    }
    orders.emplace_back("O000001", "C001");
    return getOrderById(orders, "O000001") == &orders[0] &&
           getOrderById(orders, "O000002") == nullptr
        ? "" : "Order lookup returned the wrong result.";
}

std::string testIdFormatting() {
    if (generateOrderID(1) != "O000001" || generateOrderID(999999) != "O999999") {
        return "Order ID formatting is incorrect.";
    }
    try {
        generateOrderID(0);
        return "Order ID zero was accepted.";
    } catch (const std::invalid_argument&) {
    }
    try {
        generateOrderID(1000000);
        return "Order ID above the maximum was accepted.";
    } catch (const std::out_of_range&) {
    }
    return {};
}

bool sameItems(const std::vector<OrderItem>& left, const std::vector<OrderItem>& right) {
    if (left.size() != right.size()) return false;
    for (std::size_t i = 0; i < left.size(); ++i) {
        if (left[i].getProductSKU() != right[i].getProductSKU() ||
            left[i].getProductName() != right[i].getProductName() ||
            left[i].getProductUnitPrice() != right[i].getProductUnitPrice() ||
            left[i].getOrderQuantity() != right[i].getOrderQuantity() ||
            left[i].getTotalPrice() != right[i].getTotalPrice()) return false;
    }
    return true;
}

std::string testAddItems() {
    std::vector<Order> orders{{"O000001", "C001"}, {"O000002", "C002"}};
    std::vector<Product> products{{"P001", "Notebook", 20000, 5, 1},
                                  {"P002", "Pencil", 3000, 0, 1}};
    addItemToOrder(orders, products, "O000002", "P001", 2);
    // Draft quantities do not reserve stock; availability is checked on confirmation.
    addItemToOrder(orders, products, "O000002", "P002", 3);
    const auto& items = orders[1].getItems();
    if (!orders[0].getItems().empty() || items.size() != 2 ||
        orders[1].getOrderStatus() != OrderStatus::Draft) {
        return "Adding items changed the wrong order, its status, or the expected item count.";
    }
    if (items[0].getProductSKU() != "P001" || items[0].getProductName() != "Notebook" ||
        items[0].getProductUnitPrice() != 20000 || items[0].getOrderQuantity() != 2 ||
        items[0].getTotalPrice() != 40000 || items[1].getProductSKU() != "P002" ||
        items[1].getProductName() != "Pencil" || items[1].getProductUnitPrice() != 3000 ||
        items[1].getOrderQuantity() != 3 || items[1].getTotalPrice() != 9000) {
        return "Added items did not preserve their product data, quantities, or subtotals.";
    }
    return products[0].getStockQuantity() == 5 && products[1].getStockQuantity() == 0
        ? "" : "Adding Draft items changed inventory.";
}

std::string testAddItemFailure(const std::string& scenario) {
    std::vector<Order> orders{{"O000001", "C001"}};
    std::vector<Product> products{{"P001", "Notebook", 20000, 5, 1},
                                  {"P002", "Pencil", 3000, 7, 1}};
    addItemToOrder(orders, products, "O000001", "P001", 2);
    std::string orderId = "O000001";
    std::string sku = "P002";
    int quantity = 1;
    if (scenario == "missing_order") orderId = "O000002";
    else if (scenario == "missing_product") sku = "P003";
    else if (scenario == "duplicate") {
        sku = "P001";
        products[0].setProductName("Renamed notebook");
        products[0].setUnitPrice(50000);
    }
    else if (scenario == "zero_quantity") quantity = 0;
    else if (scenario == "negative_quantity") quantity = -1;
    else if (scenario == "overflow") {
        products[1].setUnitPrice(std::numeric_limits<std::int64_t>::max());
        quantity = 2;
    }
    const auto before = orders[0].getItems();
    bool rejected = false;
    try {
        addItemToOrder(orders, products, orderId, sku, quantity);
    } catch (const std::overflow_error&) {
        if (scenario != "overflow") return "Expected invalid_argument, got overflow_error.";
        rejected = true;
    } catch (const std::invalid_argument&) {
        if (scenario == "overflow") return "Expected overflow_error, got invalid_argument.";
        rejected = true;
    }
    if (!rejected) return "Invalid item addition was accepted: " + scenario;
    if (!sameItems(before, orders[0].getItems()) || orders.size() != 1 ||
        orders[0].getOrderStatus() != OrderStatus::Draft ||
        products[0].getStockQuantity() != 5 || products[1].getStockQuantity() != 7) {
        return "Failed item addition changed existing items, order status, or inventory.";
    }
    return {};
}

std::string testAddItemSnapshot() {
    std::vector<Order> orders{{"O000001", "C001"}, {"O000002", "C002"}};
    std::vector<Product> products{{"P001", "Notebook", 20000, 5, 1}};
    addItemToOrder(orders, products, "O000001", "P001", 2);
    const auto before = orders[0].getItems();
    products[0].setProductName("New name");
    products[0].setUnitPrice(50000);
    addItemToOrder(orders, products, "O000002", "P001", 1);
    if (!sameItems(before, orders[0].getItems())) {
        return "Product edits or adding to another order changed an existing snapshot.";
    }
    const auto& item = orders[1].getItems().at(0);
    return item.getProductName() == "New name" && item.getProductUnitPrice() == 50000 &&
           item.getTotalPrice() == 50000 && products[0].getStockQuantity() == 5
        ? "" : "A later order did not capture current product values without changing stock.";
}

std::string testRemoveItem(bool removeLast) {
    std::vector<Order> orders{{"O000001", "C001"}, {"O000002", "C002"}};
    std::vector<Product> products{{"P001", "Notebook", 20000, 5, 1},
                                  {"P002", "Pencil", 3000, 7, 1}};
    addItemToOrder(orders, products, "O000001", "P001", 1);
    addItemToOrder(orders, products, "O000002", "P001", 2);
    if (!removeLast) addItemToOrder(orders, products, "O000002", "P002", 3);
    const auto unrelatedItems = orders[0].getItems();
    const std::vector<OrderItem> expectedItems = removeLast
        ? std::vector<OrderItem>{}
        : std::vector<OrderItem>{orders[1].getItems()[1]};
    // Removal must preserve the surviving historical line, even after a product edit.
    products[1].setProductName("New pencil name");
    products[1].setUnitPrice(9000);

    removeItemFromOrder(orders, "O000002", "P001");

    if (orders.size() != 2 || !sameItems(orders[1].getItems(), expectedItems) ||
        !sameItems(orders[0].getItems(), unrelatedItems)) {
        return "Removal changed the wrong items or failed to preserve the remaining snapshot.";
    }
    if (orders[0].getOrderStatus() != OrderStatus::Draft ||
        orders[1].getOrderStatus() != OrderStatus::Draft ||
        orders[0].getId() != "O000001" || orders[1].getId() != "O000002" ||
        orders[0].getCustomerId() != "C001" || orders[1].getCustomerId() != "C002") {
        return "Removal changed order identity, customer, or status.";
    }
    return products[0].getStockQuantity() == 5 && products[1].getStockQuantity() == 7
        ? "" : "Removing a Draft item changed inventory.";
}

std::string testRemoveItemFailure(const std::string& scenario) {
    std::vector<Order> orders{{"O000001", "C001"}, {"O000002", "C002"}};
    std::vector<Product> products{{"P001", "Notebook", 20000, 5, 1},
                                  {"P002", "Pencil", 3000, 7, 1}};
    addItemToOrder(orders, products, "O000001", "P001", 1);
    if (scenario != "empty_order") {
        addItemToOrder(orders, products, "O000002", "P001", 2);
    }
    const auto before = orders;
    const std::string orderId = scenario == "missing_order" ? "O000003" : "O000002";
    const std::string sku = scenario == "missing_item" ? "P002" : "P001";
    bool rejected = false;
    try {
        removeItemFromOrder(orders, orderId, sku);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    if (!rejected) return "Invalid removal was accepted: " + scenario;
    if (orders.size() != before.size()) return "Failed removal changed the order count.";
    for (std::size_t i = 0; i < orders.size(); ++i) {
        if (!sameItems(orders[i].getItems(), before[i].getItems()) ||
            orders[i].getId() != before[i].getId() ||
            orders[i].getCustomerId() != before[i].getCustomerId() ||
            orders[i].getOrderStatus() != before[i].getOrderStatus()) {
            return "Failed removal changed an order.";
        }
    }
    return products[0].getStockQuantity() == 5 && products[1].getStockQuantity() == 7
        ? "" : "Failed removal changed inventory.";
}

// Non-Draft removal tests will be added when public status transitions are available.

std::string testSubtotal() {
    const Product product{"P001", "Notebook", 20000, 5, 1};
    const OrderItem item{product, 2};
    if (item.getProductSKU() != "P001" || item.getProductName() != "Notebook" ||
        item.getProductUnitPrice() != 20000 || item.getOrderQuantity() != 2 ||
        item.getTotalPrice() != 40000) {
        return "Order item did not preserve its fields or calculate 20000 * 2 as 40000.";
    }
    return {};
}

std::string testZeroPrice() {
    const Product product{"P001", "Free sample", 0, 5, 1};
    const OrderItem item{product, 3};
    return item.getTotalPrice() == 0 ? "" : "A zero-priced item should have a zero subtotal.";
}

std::string testInvalidQuantity(int quantity) {
    const Product product{"P001", "Notebook", 20000, 5, 1};
    try {
        const OrderItem item{product, quantity};
    } catch (const std::invalid_argument&) {
        return {};
    } catch (const std::exception& error) {
        return "Expected invalid_argument, got: " + std::string{error.what()};
    }
    return "Order item accepted a non-positive quantity.";
}

std::string testSubtotalBoundary() {
    const auto max = std::numeric_limits<std::int64_t>::max();
    const Product product{"P001", "Expensive item", max / 2, 2, 0};
    const OrderItem item{product, 2};
    return item.getTotalPrice() == max - 1
        ? "" : "A subtotal just below INT64_MAX was calculated incorrectly.";
}

std::string testOverflow() {
    const auto max = std::numeric_limits<std::int64_t>::max();
    const Product product{"P001", "Expensive item", max / 2 + 1, 2, 0};
    try {
        const OrderItem item{product, 2};
    } catch (const std::overflow_error&) {
        return {};
    } catch (const std::exception& error) {
        return "Expected overflow_error, got: " + std::string{error.what()};
    }
    return "Order item accepted a subtotal above INT64_MAX.";
}

std::string testSnapshot() {
    Product product{"P001", "Original name", 20000, 5, 1};
    const OrderItem item{product, 2};
    product.setProductName("Updated name");
    product.setUnitPrice(50000);

    if (item.getProductSKU() != "P001" || item.getProductName() != "Original name" ||
        item.getProductUnitPrice() != 20000 || item.getOrderQuantity() != 2 ||
        item.getTotalPrice() != 40000) {
        return "Editing the product changed the order item's historical values.";
    }
    return {};
}

std::string testOrderTotalEmpty() {
    const Order order{"O000001", "C001"};
    return order.getSubTotal() == 0 && order.getDiscountAmount() == 0 &&
           order.getFinalTotal() == 0
        ? "" : "An empty order should have zero subtotal, discount, and final total.";
}

std::string testOrderTotalMultipleItems() {
    Order order{"O000001", "C001"};
    order.addItem(Product{"P001", "Notebook", 20000, 5, 1}, 2);
    order.addItem(Product{"P002", "Pencil", 3000, 7, 1}, 3);
    const auto before = order.getItems();
    const Order& readOnlyOrder = order;
    if (readOnlyOrder.getSubTotal() != 49000) {
        return "Order total should sum 40000 and 9000 to 49000.";
    }
    if (readOnlyOrder.getDiscountAmount() != 0 || readOnlyOrder.getFinalTotal() != 49000) {
        return "An undiscounted order should have zero discount and a final total of 49000.";
    }
    return sameItems(before, order.getItems()) && order.getOrderStatus() == OrderStatus::Draft
        ? "" : "Calculating the total changed the order.";
}

std::string testOrderTotalBoundary() {
    const auto max = std::numeric_limits<std::int64_t>::max();
    Order order{"O000001", "C001"};
    order.addItem(Product{"P001", "Expensive item", max, 1, 0}, 1);
    order.addItem(Product{"P002", "Free sample", 0, 1, 0}, 1);
    return order.getSubTotal() == max && order.getDiscountAmount() == 0 &&
           order.getFinalTotal() == max
        ? "" : "INT64_MAX plus zero should retain exact subtotal/final total and zero discount.";
}

std::string testOrderTotalOverflow(bool finalTotal = false) {
    const auto max = std::numeric_limits<std::int64_t>::max();
    Order order{"O000001", "C001"};
    order.addItem(Product{"P001", "Expensive item", max, 1, 0}, 1);
    order.addItem(Product{"P002", "One rial item", 1, 1, 0}, 1);
    const auto before = order.getItems();
    try {
        if (finalTotal) order.getFinalTotal();
        else order.getSubTotal();
    } catch (const std::overflow_error&) {
        return sameItems(before, order.getItems()) &&
               order.getOrderStatus() == OrderStatus::Draft &&
               order.getId() == "O000001" && order.getCustomerId() == "C001"
            ? "" : "A rejected total calculation changed the order.";
    } catch (const std::exception& error) {
        return "Expected overflow_error for the order total, got: " + std::string{error.what()};
    }
    return "Order total accepted INT64_MAX plus one.";
}

std::string testConfirmationValidation(const std::string& scenario) {
    std::vector<Product> products{{"P001", "Notebook", 20000, 5, 1},
                                  {"P002", "Pencil", 3000, 7, 1}};
    Order order{"O000001", "C001"};
    if (scenario != "empty") {
        order.addItem(products[0], scenario == "exact_stock" ? 5 : 2);
        order.addItem(products[1], scenario == "insufficient_stock" ? 8 : 7);
    }
    // The order retains its snapshot even when its product is absent from this inventory.
    if (scenario == "missing_product") products.pop_back();
    const auto productsBefore = products;
    const auto orderBefore = order;
    const bool shouldFail = scenario == "empty" || scenario == "missing_product" ||
                            scenario == "insufficient_stock";
    bool rejected = false;
    std::string message;
    try {
        validateOrderForConfirmation(order, products);
    } catch (const std::invalid_argument& error) {
        if (scenario != "missing_product") return "Unexpected invalid_argument during validation.";
        rejected = true;
        message = error.what();
    } catch (const std::logic_error& error) {
        if (scenario != "empty" && scenario != "insufficient_stock") {
            return "Unexpected logic_error during validation.";
        }
        rejected = true;
        message = error.what();
    }
    if (rejected != shouldFail) return "Unexpected validation outcome: " + scenario;
    if (!sameItems(order.getItems(), orderBefore.getItems()) ||
        order.getOrderStatus() != orderBefore.getOrderStatus() ||
        order.getId() != orderBefore.getId() || order.getCustomerId() != orderBefore.getCustomerId() ||
        order.getSubTotal() != orderBefore.getSubTotal() ||
        order.getDiscountAmount() != orderBefore.getDiscountAmount() ||
        order.getFinalTotal() != orderBefore.getFinalTotal()) {
        return "Confirmation validation changed the order.";
    }
    if (products.size() != productsBefore.size()) return "Validation changed the inventory size.";
    for (std::size_t i = 0; i < products.size(); ++i) {
        const auto& before = productsBefore[i];
        const auto& after = products[i];
        if (after.getSku() != before.getSku() || after.getProductName() != before.getProductName() ||
            after.getUnitPrice() != before.getUnitPrice() ||
            after.getStockQuantity() != before.getStockQuantity() ||
            after.getReorderLevel() != before.getReorderLevel() || after.isArchived() != before.isArchived()) {
            return "Confirmation validation changed inventory.";
        }
    }
    if (shouldFail && message.find("O000001") == std::string::npos) {
        return "Validation error did not identify the order.";
    }
    if ((scenario == "missing_product" || scenario == "insufficient_stock") &&
        message.find("P002") == std::string::npos) {
        return "Validation error did not identify the failing second product.";
    }
    if (scenario == "insufficient_stock" &&
        (message.find("Requested: 8") == std::string::npos ||
         message.find("available: 7") == std::string::npos)) {
        return "Insufficient-stock error omitted the requested or available quantity.";
    }
    return {};
}

std::string testConfirmOrder(const std::string& scenario) {
    std::vector<Product> products{{"P001", "Notebook", 20000, 5, 1},
                                  {"P002", "Pencil", 3000, 7, 1},
                                  {"P003", "Unrelated product", 100, 9, 0}};
    std::vector<Order> orders{{"O000001", "C001"}, {"O000002", "C002"}};
    orders[0].addItem(products[2], 1);
    if (scenario == "overflow") {
        products[0].setUnitPrice(std::numeric_limits<std::int64_t>::max());
        products[1].setUnitPrice(1);
    }
    const int firstQuantity = scenario == "exact_stock" ? 5 : scenario == "overflow" ? 1 : 2;
    const int secondQuantity = scenario == "exact_stock" ? 7 : scenario == "overflow" ? 1 : 3;
    if (scenario != "empty") {
        orders[1].addItem(products[0], firstQuantity);
        orders[1].addItem(products[1], secondQuantity);
    }
    // Current product edits must not reprice historical order lines on confirmation.
    products[0].setProductName("Renamed notebook");
    products[0].setUnitPrice(50000);
    if (scenario == "archived") products[1].archive();
    if (scenario == "insufficient_stock") products[1].decreaseStock(5);
    if (scenario == "missing_product") products.erase(products.begin() + 1);
    if (scenario == "repeated") confirmOrder(orders, products, "O000002");

    const auto ordersBefore = orders;
    const auto productsBefore = products;
    const bool shouldSucceed = scenario == "valid" || scenario == "exact_stock";
    bool rejected = false;
    try {
        confirmOrder(orders, products, scenario == "missing_order" ? "O000003" : "O000002");
    } catch (const std::invalid_argument&) {
        if (scenario != "missing_order" && scenario != "missing_product") {
            return "Unexpected invalid_argument during confirmation: " + scenario;
        }
        rejected = true;
    } catch (const std::overflow_error&) {
        if (scenario != "overflow") return "Unexpected overflow_error during confirmation.";
        rejected = true;
    } catch (const std::logic_error&) {
        if (scenario != "empty" && scenario != "archived" &&
            scenario != "insufficient_stock" && scenario != "repeated") {
            return "Unexpected logic_error during confirmation: " + scenario;
        }
        rejected = true;
    }
    if (rejected == shouldSucceed) return "Unexpected confirmation outcome: " + scenario;
    if (orders.size() != ordersBefore.size() || products.size() != productsBefore.size()) {
        return "Confirmation changed collection sizes.";
    }
    for (std::size_t i = 0; i < orders.size(); ++i) {
        const auto expectedStatus = shouldSucceed && i == 1
            ? OrderStatus::Confirmed : ordersBefore[i].getOrderStatus();
        if (orders[i].getOrderStatus() != expectedStatus ||
            orders[i].getId() != ordersBefore[i].getId() ||
            orders[i].getCustomerId() != ordersBefore[i].getCustomerId() ||
            !sameItems(orders[i].getItems(), ordersBefore[i].getItems())) {
            return "Confirmation changed unexpected order data or produced the wrong status.";
        }
        // Overflowing snapshots cannot be summed; sameItems above still checks every line.
        if (scenario != "overflow" &&
            (orders[i].getSubTotal() != ordersBefore[i].getSubTotal() ||
             orders[i].getDiscountAmount() != ordersBefore[i].getDiscountAmount() ||
             orders[i].getFinalTotal() != ordersBefore[i].getFinalTotal())) {
            return "Confirmation changed historical totals.";
        }
    }
    for (std::size_t i = 0; i < products.size(); ++i) {
        const auto& before = productsBefore[i];
        const auto& after = products[i];
        int expectedStock = before.getStockQuantity();
        if (shouldSucceed && before.getSku() == "P001") expectedStock -= firstQuantity;
        if (shouldSucceed && before.getSku() == "P002") expectedStock -= secondQuantity;
        if (after.getStockQuantity() != expectedStock || after.getSku() != before.getSku() ||
            after.getProductName() != before.getProductName() ||
            after.getUnitPrice() != before.getUnitPrice() ||
            after.getReorderLevel() != before.getReorderLevel() || after.isArchived() != before.isArchived()) {
            return "Confirmation changed unexpected inventory data: " + scenario;
        }
    }
    return {};
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Provide one test name.\n";
        return 1;
    }
    const std::string name{argv[1]};
    std::string error;
    try {
        if (name == "new_draft") error = testNewDraft();
        else if (name == "missing_customer") error = testMissingCustomer();
        else if (name == "unique_ids") error = testUniqueIds();
        else if (name == "next_id_unordered") error = testNextIdUnordered();
        else if (name == "id_exhausted") error = testIdExhausted();
        else if (name == "id_validation") error = testIdValidation();
        else if (name == "lookup") error = testLookup();
        else if (name == "id_formatting") error = testIdFormatting();
        else if (name == "add_items") error = testAddItems();
        else if (name == "add_snapshot") error = testAddItemSnapshot();
        else if (name == "add_missing_order") error = testAddItemFailure("missing_order");
        else if (name == "add_missing_product") error = testAddItemFailure("missing_product");
        else if (name == "add_duplicate") error = testAddItemFailure("duplicate");
        else if (name == "add_zero_quantity") error = testAddItemFailure("zero_quantity");
        else if (name == "add_negative_quantity") error = testAddItemFailure("negative_quantity");
        else if (name == "add_overflow") error = testAddItemFailure("overflow");
        else if (name == "remove_item") error = testRemoveItem(false);
        else if (name == "remove_last_item") error = testRemoveItem(true);
        else if (name == "remove_missing_item") error = testRemoveItemFailure("missing_item");
        else if (name == "remove_empty_order") error = testRemoveItemFailure("empty_order");
        else if (name == "remove_missing_order") error = testRemoveItemFailure("missing_order");
        else if (name == "total_empty") error = testOrderTotalEmpty();
        else if (name == "total_multiple_items") error = testOrderTotalMultipleItems();
        else if (name == "total_boundary") error = testOrderTotalBoundary();
        else if (name == "total_overflow") error = testOrderTotalOverflow();
        else if (name == "final_total_overflow") error = testOrderTotalOverflow(true);
        else if (name == "validate_valid") error = testConfirmationValidation("valid");
        else if (name == "validate_exact_stock") error = testConfirmationValidation("exact_stock");
        else if (name == "validate_empty") error = testConfirmationValidation("empty");
        else if (name == "validate_missing_product") error = testConfirmationValidation("missing_product");
        else if (name == "validate_insufficient_stock") error = testConfirmationValidation("insufficient_stock");
        else if (name == "confirm_valid") error = testConfirmOrder("valid");
        else if (name == "confirm_exact_stock") error = testConfirmOrder("exact_stock");
        else if (name == "confirm_empty") error = testConfirmOrder("empty");
        else if (name == "confirm_missing_order") error = testConfirmOrder("missing_order");
        else if (name == "confirm_missing_product") error = testConfirmOrder("missing_product");
        else if (name == "confirm_archived") error = testConfirmOrder("archived");
        else if (name == "confirm_insufficient_stock") error = testConfirmOrder("insufficient_stock");
        else if (name == "confirm_overflow") error = testConfirmOrder("overflow");
        else if (name == "confirm_repeated") error = testConfirmOrder("repeated");
        else if (name == "subtotal") error = testSubtotal();
        else if (name == "zero_price") error = testZeroPrice();
        else if (name == "zero_quantity") error = testInvalidQuantity(0);
        else if (name == "negative_quantity") error = testInvalidQuantity(-1);
        else if (name == "subtotal_boundary") error = testSubtotalBoundary();
        else if (name == "overflow") error = testOverflow();
        else if (name == "snapshot") error = testSnapshot();
        else error = "Unknown test name: " + name;
    } catch (const std::exception& exception) {
        error = "Unexpected exception: " + std::string{exception.what()};
    }
    if (!error.empty()) {
        std::cerr << error << '\n';
        return 1;
    }
    return 0;
}
