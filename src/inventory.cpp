#include "inventory.h"
#include <iomanip>
#include <sstream>
#include <stdexcept>

Product* getProductBySKU(std::vector<Product>& products, const std::string& sku) {
    for (auto& product : products) {
        if (product.getSku() == sku) {
            return &product;
        }
    }
    return nullptr;
}

void addProduct(std::vector<Product> &products, const Product &product) {
    if (getProductBySKU(products, product.getSku())) {
        throw std::invalid_argument("there exists a product with matching SKU.");
    }
    products.push_back(product);
}

std::string generateSKU(std::uint32_t id) {
    if (id > 0xFFF) {
        throw std::out_of_range("SKU limit exceeded");
    }
    if (id == 0) {
        throw std::out_of_range("000 is reserved for testing");
    }

    std::ostringstream oss;
    oss << 'P' << std::uppercase << std::hex << std::setw(3) << std::setfill('0') << id;

    return oss.str();
}

std::uint32_t findNextID(const std::vector<Product>& products) {
    if (products.empty()) {
        return 1;
    }

    std::uint32_t nextID{0};
    // Products may be reordered, and archived SKUs remain reserved, so scan all entries.
    for (const auto& product : products) {
        const std::string sku = product.getSku();
        if (sku.size() != 4 || sku[0] != 'P') {
            throw std::invalid_argument("Invalid SKU format");
        }
        std::size_t pos{0};
        auto id = std::stoul(sku.substr(1), &pos, 16);

        if (pos != 3) {
            throw std::invalid_argument("Invalid Hexadecimal digits");
        }

        if (id >= nextID) {
            nextID = static_cast<std::uint32_t>(id + 1);
        }
    }
    if (nextID > 0xFFF) {
        throw std::out_of_range("Maximum SKU limit reached");
    }
    return nextID;
}

std::vector<const Product*> getLowStockProducts(const std::vector<Product>& products) {
    std::vector<const Product*> lowStockProducts;
    for (const Product& product : products) {
        if (product.isLowStock() && !product.isArchived()) {
            lowStockProducts.push_back(&product);
        }
    }
    return lowStockProducts;
}
