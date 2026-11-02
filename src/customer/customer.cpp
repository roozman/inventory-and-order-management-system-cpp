#include "customer.h"
#include <stdexcept>

Customer::Customer(std::string id, std::string name): id_{id}, name_{name} {
    // Validate here as well as in the ID generator: callers can construct a Customer directly.
    if (id.size() != 4 || id[0] != 'C'
    || id == "C000" || id.substr(1).find_first_not_of("0123456789ABCDEF") != std::string::npos) {
        throw std::invalid_argument("Invalid Customer ID: expected C001 through CFFF using uppercase hexadecimal digits.");
    }
    if (!customerNameValidator(name)) {
        throw std::invalid_argument("Customer name cannot be empty or whitespace-only.");
    }
}

bool Customer::customerNameValidator(const std::string& name) {
    return name.find_first_not_of(" \t\n\r\f\v") != std::string::npos;
}

std::string Customer::getId() const {
    return id_;
}

std::string Customer::getName() const {
    return name_;
}
