#include "product_storage.h"
#include <fstream>
#include <stdexcept>
#include <filesystem>

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
            std::to_string(product.getReorderLevel())  + ',' +
            std::to_string(product.getDiscount()) + ',' + (product.isArchived()?"1":"0") + "\n";

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