#include "customer_storage.h"
#include "storage_helpers.h"
#include <stdexcept>
namespace {
const std::vector<std::string> header{"customer_id", "name"};
}
void saveCustomers(const std::vector<Customer>& customers, const std::string& path) {
    CsvRows rows;
    for (const auto& customer : customers) rows.push_back({customer.getId(), customer.getName()});
    writeCsvTable(path, header, rows);
}
std::vector<Customer> loadCustomers(const std::string& path) {
    const auto rows = readCsvTable(path, header);
    std::vector<Customer> customers;
    for (std::size_t i = 0; i < rows.size(); ++i) {
        try {
            Customer customer{rows[i][0], rows[i][1]};
            for (const auto& existing : customers)
                if (existing.getId() == customer.getId()) throw std::invalid_argument("Duplicate customer ID.");
            customers.push_back(customer);
        } catch (const std::exception& error) {
            throw std::runtime_error(path + ", record " + std::to_string(i + 2) + ": " + error.what());
        }
    }
    return customers;
}
