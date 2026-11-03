#include "product_storage.h"
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <limits>

void createProductHeader(std::ofstream &file) {
    file << "sku,name,unit_price,stock_quantity,reorder_level,discount_percent,archived\n";
}

std::string escapeCsvField(const std::string &text) {
    std::string field{};
    field += '"';
    for (const char ch : text) {
        if (ch == '"') {
            field += '"';
        }
        field += ch;
    }
    field += '"';

    return field;
}

void saveProducts(const std::vector<Product> &products, const std::string &filePath) {
    recoverProductSave(filePath);
    const std::string tempPath = filePath + ".tmp";
    std::ofstream file(tempPath, std::ios::binary);

    if (!file) {
        throw std::runtime_error("Cannot open product file: " + tempPath);
    }

    createProductHeader(file);
    for (const auto& product : products) {
        std::string row{};
        row = product.getSku() + ',' + escapeCsvField(product.getProductName()) + ',' +
            std::to_string(product.getUnitPrice()) + ',' + std::to_string(product.getStockQuantity()) + ',' +
            std::to_string(product.getReorderLevel()) + ',' +
            std::to_string(product.getDiscount()) + ',' + (product.isArchived() ? "1" : "0") + "\n";

        file << row;
    }
    file.close();
    if (!file) {
        throw std::runtime_error("Failed to write product file: " + tempPath);
    }

    const std::string pendingPath = filePath + ".pending";

    std::ofstream pendingFile{pendingPath};
    if (!pendingFile) {
        throw std::runtime_error("Cannot create pending file: " + pendingPath);
    }

    pendingFile.close();
    if (!pendingFile) {
        throw std::runtime_error("Failed to close pending file: " + pendingPath);
    }

    recoverProductSave(filePath);
}

void recoverProductSave(const std::string& filePath) {
    const std::string tempPath = filePath + ".tmp";
    const std::string pendingPath = filePath + ".pending";

    if (!std::filesystem::exists(pendingPath)) {
        return;
    }

    std::filesystem::copy_file(
        tempPath,
        filePath,
        std::filesystem::copy_options::overwrite_existing
    );

    std::filesystem::remove(pendingPath);
}

bool readCsvRecord(std::istream& input, std::vector<std::string>& fields) {
    enum class State {
        FieldStart,
        Unquoted,
        Quoted,
        AfterQuote
    };

    fields.clear();
    std::string field;
    State state{State::FieldStart};
    bool readAnything{false};
    char ch{};

    while (input.get(ch)) {
        readAnything = true;

        if (state == State::Quoted) {
            if (ch == '"') {
                state = State::AfterQuote;
            }
            else {
                field += ch;
            }
            continue;
        }

        if (state == State::AfterQuote && ch == '"') {
            field += '"';
            state = State::Quoted;
            continue;
        }

        if (ch == ',') {
            fields.push_back(field);
            field.clear();
            state = State::FieldStart;
        }
        else if (ch == '\n' || ch == '\r') {
            if (ch == '\r') {
                if (input.peek() != '\n') {
                    throw std::runtime_error("Expected LF after CR.");
                }
                input.get(ch);
                if (!input) {
                    throw std::runtime_error("Failed to read CSV line ending.");
                }
            }

            fields.push_back(field);
            return true;
        }
        else if (state == State::AfterQuote) {
            throw std::runtime_error(
                "Unexpected character after closing quote."
            );
        }
        else if (ch == '"') {
            if (state != State::FieldStart) {
                throw std::runtime_error(
                    "Unexpected quote inside an unquoted field."
                );
            }
            state = State::Quoted;
        }
        else {
            field += ch;
            state = State::Unquoted;
        }
    }

    if (input.bad() || !input.eof()) {
        throw std::runtime_error("Failed to read CSV record.");
    }

    if (state == State::Quoted) {
        throw std::runtime_error("Unclosed quoted field.");
    }

    if (!readAnything) {
        return false;
    }

    // Accept a final record without a trailing newline.
    fields.push_back(field);
    return true;
}

std::int64_t parseNonNegativeNumber(const std::string& text,
                                   const std::string& fieldName,
                                   std::int64_t maximum) {
    if (text.empty() ||
        text.find_first_not_of("0123456789") != std::string::npos) {
        throw std::invalid_argument(
            fieldName + " must contain only decimal digits."
        );
    }

    std::int64_t value{};
    try {
        value = std::stoll(text);
    }
    catch (const std::out_of_range&) {
        throw std::out_of_range(fieldName + " is too large.");
    }

    if (value > maximum) {
        throw std::out_of_range(fieldName + " exceeds its allowed maximum.");
    }

    return value;
}

std::vector<Product> loadProducts(const std::string& filePath) {
    recoverProductSave(filePath);

    std::ifstream file{filePath, std::ios::binary};
    if (!file) {
        throw std::runtime_error("Cannot open product file: " + filePath);
    }

    const std::vector<std::string> expectedHeader{
        "sku", "name", "unit_price", "stock_quantity",
        "reorder_level", "discount_percent", "archived"
    };

    std::vector<std::string> fields;
    try {
        if (!readCsvRecord(file, fields) || fields != expectedHeader) {
            throw std::runtime_error("Invalid or missing product header.");
        }
    }
    catch (const std::runtime_error& error) {
        throw std::runtime_error(
            filePath + ", record 1: " + error.what()
        );
    }

    std::vector<Product> products;

    std::size_t recordNumber{1}; // The header is record 1.

    while (true) {
        ++recordNumber;

        try {
            if (!readCsvRecord(file, fields)) {
                break;
            }

            if (fields.size() != 7) {
                throw std::invalid_argument("Expected exactly seven fields.");
            }

            const auto unitPrice = parseNonNegativeNumber(
                fields[2], "Unit price",
                std::numeric_limits<std::int64_t>::max()
            );

            const int stock = static_cast<int>(parseNonNegativeNumber(
                fields[3], "Stock quantity", std::numeric_limits<int>::max()
            ));

            const int reorderLevel = static_cast<int>(parseNonNegativeNumber(
                fields[4], "Reorder level", std::numeric_limits<int>::max()
            ));

            const int discount = static_cast<int>(parseNonNegativeNumber(
                fields[5], "Discount percentage", 100
            ));

            if (fields[6] != "0" && fields[6] != "1") {
                throw std::invalid_argument("Archived must be 0 or 1.");
            }

            for (const auto& existing : products) {
                if (existing.getSku() == fields[0]) {
                    throw std::invalid_argument(
                        "Duplicate SKU: " + fields[0]
                    );
                }
            }

            Product product{
                fields[0], fields[1], unitPrice, stock, reorderLevel
            };

            product.setDiscount(discount);

            if (fields[6] == "1") {
                product.archive();
            }

            products.push_back(product);
        }
        catch (const std::logic_error& error) {
            throw std::runtime_error(
                filePath + ", record " + std::to_string(recordNumber) +
                ": " + error.what()
            );
        }
        catch (const std::runtime_error& error) {
            throw std::runtime_error(
                filePath + ", record " + std::to_string(recordNumber) +
                ": " + error.what()
            );
        }
    }

    return products;
}

