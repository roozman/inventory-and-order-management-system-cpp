#include "customer.h"
#include "customer_registry.h"
#include <stdexcept>
#include <iomanip>
#include <sstream>

Customer *getCustomerById(std::vector<Customer> &customers, const std::string &id) {
    for (auto& customer : customers) {
        if (customer.getId() == id) {
            return &customer;
        }
    }
    return nullptr;
}

void addCustomer(std::vector<Customer> &customers, const Customer &customer) {
    if (getCustomerById(customers, customer.getId())) {
        throw std::invalid_argument("A customer with this ID already exists.");
    }
    customers.push_back(customer);
}

std::string generateCustomerId(std::uint32_t id) {
    if (id > 0xFFF) {
        throw std::out_of_range("Provided customer id exceeds id limits.");
    }
    if (id == 0) {
        throw std::invalid_argument("ID 0 is reserved for debug purposes.");
    }

    std::ostringstream oss;
    oss << "C" << std::uppercase << std::hex << std::setw(3) << std::setfill('0') << id;

    return oss.str();
}

std::uint32_t findNextCustomerId(const std::vector<Customer> &customers) {
    if (customers.empty()) {
        return 1;
    }

    std::uint32_t nextID{0};

    // Registration order need not match ID order, so the last element is not necessarily the highest ID.
    for (auto& customer : customers) {
        const std::string customerId = customer.getId();
        if (customerId.size() != 4 || customerId[0] != 'C') {
            throw std::invalid_argument("Invalid customer ID format");
        }
        std::size_t pos{0};
        auto id = std::stoul(customerId.substr(1), &pos, 16);

        if (pos != 3) {
            throw std::invalid_argument("Invalid Hexadecimal digits");
        }

        if (id >= nextID) {
            nextID = static_cast<std::uint32_t>(id + 1);
        }
    }
    if (nextID > 0xFFF) {
        throw std::out_of_range("Maximum customer ID limit reached");
    }
    return nextID;
}
