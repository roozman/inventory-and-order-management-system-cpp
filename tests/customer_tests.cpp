#include "../src/customer.h"
#include "../src/customer_registry.h"

#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::string expectInvalidCustomer(const std::string& id, const std::string& name) {
    try {
        const Customer customer{id, name};
    } catch (const std::invalid_argument&) {
        return {};
    } catch (const std::exception& error) {
        return "Expected invalid_argument, got: " + std::string{error.what()};
    }
    return "Expected invalid_argument for customer ID '" + id + "' and name '" + name + "'.";
}

std::string testValid() {
    const Customer first{"C001", "Alice Smith"};
    const Customer last{"CFFF", "  Bob  "};
    if (first.getId() != "C001" || first.getName() != "Alice Smith") {
        return "Customer did not retain its ID and name.";
    }
    if (last.getId() != "CFFF" || last.getName() != "  Bob  ") {
        return "The maximum ID or a name with surrounding spaces was not retained.";
    }
    return {};
}

std::string testInvalidIds() {
    const std::vector<std::string> ids{
        "", "C000", "C01", "C0001", "P001", "c001", "C00a", "C00G", "C 01"
    };
    for (const auto& id : ids) {
        const auto error = expectInvalidCustomer(id, "Alice");
        if (!error.empty()) return error;
    }
    return {};
}

std::string testInvalidNames() {
    for (const auto& name : {"", " \t\n\r"}) {
        const auto error = expectInvalidCustomer("C001", name);
        if (!error.empty()) return error;
    }
    return {};
}

std::string testLookupEmpty() {
    std::vector<Customer> customers;
    return getCustomerById(customers, "C001") == nullptr ? "" : "Empty registry returned a customer.";
}

std::string testLookupExisting() {
    std::vector<Customer> customers{{"C001", "Alice"}, {"C002", "Bob"}};
    return getCustomerById(customers, "C002") == &customers[1]
        ? "" : "Lookup did not return the stored customer.";
}

std::string testLookupMissing() {
    std::vector<Customer> customers{{"C001", "Alice"}};
    return getCustomerById(customers, "C002") == nullptr
        ? "" : "Lookup returned a customer for an unknown ID.";
}

std::string testAdd() {
    std::vector<Customer> customers;
    addCustomer(customers, Customer{"C001", "Alice"});
    addCustomer(customers, Customer{"C002", "Bob"});
    if (customers.size() != 2 || customers[0].getName() != "Alice" ||
        customers[1].getId() != "C002" || customers[1].getName() != "Bob") {
        return "Adding customers did not preserve both records.";
    }
    return {};
}

std::string testAddDuplicate() {
    std::vector<Customer> customers{{"C001", "Alice"}};
    try {
        addCustomer(customers, Customer{"C001", "Different name"});
    } catch (const std::invalid_argument&) {
        return customers.size() == 1 && customers[0].getName() == "Alice"
            ? "" : "Duplicate registration changed the existing record.";
    } catch (const std::exception& error) {
        return "Expected invalid_argument, got: " + std::string{error.what()};
    }
    return "Duplicate customer ID was accepted.";
}

std::string testIdFormatting() {
    return generateCustomerId(1) == "C001" &&
           generateCustomerId(42) == "C02A" &&
           generateCustomerId(4095) == "CFFF"
        ? "" : "Customer ID formatting is incorrect.";
}

std::string testIdInvalidRange() {
    try {
        generateCustomerId(0);
        return "ID zero was accepted.";
    } catch (const std::invalid_argument&) {
    }
    try {
        generateCustomerId(4096);
        return "ID above CFFF was accepted.";
    } catch (const std::out_of_range&) {
    }
    return {};
}

std::string testNextIdEmpty() {
    const std::vector<Customer> customers;
    return findNextCustomerId(customers) == 1 ? "" : "Empty registry did not start at ID 1.";
}

std::string testNextIdUnordered() {
    const std::vector<Customer> customers{{"C00A", "Alice"}, {"C002", "Bob"}};
    return findNextCustomerId(customers) == 11
        ? "" : "Next ID should follow the largest ID, regardless of vector order.";
}

std::string testNextIdExhausted() {
    const std::vector<Customer> customers{{"CFFF", "Alice"}};
    try {
        findNextCustomerId(customers);
    } catch (const std::out_of_range&) {
        return {};
    }
    return "Exhausted customer ID range was accepted.";
}

std::string testConsecutiveRegistration() {
    std::vector<Customer> customers;
    for (const auto& name : {"Alice", "Bob"}) {
        const std::uint32_t next = findNextCustomerId(customers);
        addCustomer(customers, Customer{generateCustomerId(next), name});
    }
    return customers.size() == 2 && customers[0].getId() == "C001" &&
           customers[1].getId() == "C002"
        ? "" : "Consecutive registration did not assign C001 and C002.";
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
        if (testName == "valid") error = testValid();
        else if (testName == "invalid_ids") error = testInvalidIds();
        else if (testName == "invalid_names") error = testInvalidNames();
        else if (testName == "lookup_empty") error = testLookupEmpty();
        else if (testName == "lookup_existing") error = testLookupExisting();
        else if (testName == "lookup_missing") error = testLookupMissing();
        else if (testName == "add") error = testAdd();
        else if (testName == "add_duplicate") error = testAddDuplicate();
        else if (testName == "id_formatting") error = testIdFormatting();
        else if (testName == "id_invalid_range") error = testIdInvalidRange();
        else if (testName == "next_id_empty") error = testNextIdEmpty();
        else if (testName == "next_id_unordered") error = testNextIdUnordered();
        else if (testName == "next_id_exhausted") error = testNextIdExhausted();
        else if (testName == "consecutive_registration") error = testConsecutiveRegistration();
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
