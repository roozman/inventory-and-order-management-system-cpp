#include "../src/order_item.h"

#include <cstdint>
#include <exception>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

namespace {

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

} // namespace

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Provide one test name.\n";
        return 1;
    }

    const std::string testName{argv[1]};
    std::string error;
    try {
        if (testName == "subtotal") error = testSubtotal();
        else if (testName == "zero_price") error = testZeroPrice();
        else if (testName == "zero_quantity") error = testInvalidQuantity(0);
        else if (testName == "negative_quantity") error = testInvalidQuantity(-1);
        else if (testName == "subtotal_boundary") error = testSubtotalBoundary();
        else if (testName == "overflow") error = testOverflow();
        else if (testName == "snapshot") error = testSnapshot();
        else error = "Unknown test name: " + testName;
    } catch (const std::exception& exception) {
        error = "Unexpected exception: " + std::string{exception.what()};
    }

    if (!error.empty()) {
        std::cerr << error << '\n';
        return 1;
    }
    return 0;
}
