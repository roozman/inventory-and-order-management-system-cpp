#include "order_storage.h"
#include "storage_helpers.h"
#include <limits>
#include <stdexcept>
namespace {
const std::vector<std::string> orderHeader{
    "order_id", "customer_id", "status", "item_count", "subtotal", "discount_amount", "final_total"};
const std::vector<std::string> itemHeader{
    "order_id", "sku", "product_name", "unit_price", "quantity", "discount_percent",
    "subtotal", "discount_amount", "final_total"};
std::string statusName(OrderStatus status) {
    switch (status) {
        case OrderStatus::Draft: return "Draft";
        case OrderStatus::Confirmed: return "Confirmed";
        case OrderStatus::Completed: return "Completed";
        case OrderStatus::Cancelled: return "Cancelled";
    }
    throw std::invalid_argument("Invalid order status.");
}
OrderStatus parseStatus(const std::string& text) {
    if (text == "Draft") return OrderStatus::Draft;
    if (text == "Confirmed") return OrderStatus::Confirmed;
    if (text == "Completed") return OrderStatus::Completed;
    if (text == "Cancelled") return OrderStatus::Cancelled;
    throw std::invalid_argument("Unknown order status: " + text);
}
std::int64_t amount(const std::string& text) {
    return parseStorageNumber(text, std::numeric_limits<std::int64_t>::max());
}
}
void saveOrders(const std::vector<Order>& orders, const std::string& ordersPath, const std::string& itemsPath) {
    CsvRows orderRows, itemRows;
    for (const auto& order : orders) {
        orderRows.push_back({order.getId(), order.getCustomerId(), statusName(order.getOrderStatus()),
            std::to_string(order.getItems().size()), std::to_string(order.getSubTotal()),
            std::to_string(order.getDiscountAmount()), std::to_string(order.getFinalTotal())});
        for (const auto& item : order.getItems()) {
            itemRows.push_back({order.getId(), item.getProductSKU(), item.getProductName(),
                std::to_string(item.getProductUnitPrice()), std::to_string(item.getOrderQuantity()),
                std::to_string(item.getDiscount()), std::to_string(item.getTotalPrice()),
                std::to_string(item.getDiscountAmount()), std::to_string(item.getFinalTotal())});
        }
    }
    writeCsvTable(ordersPath, orderHeader, orderRows);
    writeCsvTable(itemsPath, itemHeader, itemRows);
}
std::vector<Order> loadOrders(const std::string& ordersPath, const std::string& itemsPath,
                             const std::vector<Customer>& customers, const std::vector<Product>& products) {
    const auto orderRows = readCsvTable(ordersPath, orderHeader);
    const auto itemRows = readCsvTable(itemsPath, itemHeader);
    std::vector<std::vector<OrderItem>> items(orderRows.size());
    std::vector<Order> orders;
    for (std::size_t i = 0; i < orderRows.size(); ++i) {
        try {
            const auto& row = orderRows[i];
            Order validated{row[0], row[1]};
            bool found = false;
            for (const auto& customer : customers) if (customer.getId() == row[1]) found = true;
            if (!found) throw std::invalid_argument("Missing customer: " + row[1]);
            for (std::size_t j = 0; j < i; ++j)
                if (orderRows[j][0] == row[0]) throw std::invalid_argument("Duplicate order ID.");
            parseStatus(row[2]);
        } catch (const std::exception& error) {
            throw std::runtime_error(ordersPath + ", record " + std::to_string(i + 2) + ": " + error.what());
        }
    }
    for (std::size_t i = 0; i < itemRows.size(); ++i) {
        try {
            const auto& row = itemRows[i];
            std::size_t index = 0;
            while (index < orderRows.size() && orderRows[index][0] != row[0]) ++index;
            if (index == orderRows.size()) throw std::invalid_argument("Missing order: " + row[0]);
            bool found = false;
            for (const auto& product : products) if (product.getSku() == row[1]) found = true;
            if (!found) throw std::invalid_argument("Missing product: " + row[1]);
            // Build a snapshot from saved fields, never the current inventory values.
            Product snapshot{row[1], row[2], amount(row[3]), 0, 0};
            snapshot.setDiscount(static_cast<int>(parseStorageNumber(row[5], 100)));
            OrderItem item{snapshot, static_cast<int>(parseStorageNumber(row[4], std::numeric_limits<int>::max()))};
            if (item.getTotalPrice() != amount(row[6]) || item.getDiscountAmount() != amount(row[7]) ||
                item.getFinalTotal() != amount(row[8])) throw std::invalid_argument("Saved line amounts do not match.");
            for (const auto& existing : items[index])
                if (existing.getProductSKU() == item.getProductSKU()) throw std::invalid_argument("Duplicate item SKU.");
            items[index].push_back(item);
        } catch (const std::exception& error) {
            throw std::runtime_error(itemsPath + ", record " + std::to_string(i + 2) + ": " + error.what());
        }
    }
    for (std::size_t i = 0; i < orderRows.size(); ++i) {
        try {
            const auto& row = orderRows[i];
            if (static_cast<std::uint64_t>(amount(row[3])) != items[i].size())
                throw std::invalid_argument("Saved item count does not match.");
            auto order = Order::restore(row[0], row[1], parseStatus(row[2]), items[i]);
            if (order.getSubTotal() != amount(row[4]) || order.getDiscountAmount() != amount(row[5]) ||
                order.getFinalTotal() != amount(row[6])) throw std::invalid_argument("Saved order totals do not match.");
            orders.push_back(order);
        } catch (const std::exception& error) {
            throw std::runtime_error(ordersPath + ", record " + std::to_string(i + 2) + ": " + error.what());
        }
    }
    return orders;
}
