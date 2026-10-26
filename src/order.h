#ifndef STORE_ORDER_H
#define STORE_ORDER_H

#include <string>
#include <vector>
#include "order_item.h"
#include "customer.h"

enum class OrderStatus {
    Draft,
    Confirmed,
    Completed,
    Cancelled
};

class Order {
private:
    std::string orderId_;
    std::string customerId_;
    OrderStatus orderStatus_{OrderStatus::Draft};
    std::vector<OrderItem> orderItems_{};

public:
    // Requires O000001 through O999999; rejects invalid IDs with std::invalid_argument.
    // Customer existence is checked by addDraftOrder, not by this constructor.
    Order(std::string orderId, std::string customerId);
    std::string getId() const;
    OrderStatus getOrderStatus() const;
    std::string getCustomerId() const;
    std::int64_t getDiscountAmount() const;
    const std::vector<OrderItem>& getItems() const;
    void addItem(const Product& product, int quantity);
    void removeItem(const std::string& sku);
    std::int64_t getSubTotal() const;
    std::int64_t getFinalTotal() const;
    void confirm(std::vector<Product>& products);
    void cancelDraft();
    void cancelConfirmed(std::vector<Product>& products);
    void complete();
};

#endif //STORE_ORDER_H
