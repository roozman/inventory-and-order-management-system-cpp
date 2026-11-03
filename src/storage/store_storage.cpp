#include "store_storage.h"
#include "product_storage.h"
#include "customer_storage.h"
#include "order_storage.h"
#include <array>
#include <filesystem>
#include <fstream>
#include <stdexcept>
namespace {
const std::array<std::string, 4> files{"products.csv", "customers.csv", "orders.csv", "order_items.csv"};
StoreData readDataset(const std::filesystem::path& directory) {
    StoreData data;
    data.products = loadProducts((directory / files[0]).string());
    data.customers = loadCustomers((directory / files[1]).string());
    data.orders = loadOrders((directory / files[2]).string(), (directory / files[3]).string(),
                             data.customers, data.products);
    return data;
}
}
void recoverStoreSave(const std::string& directory) {
    const std::filesystem::path root{directory};
    if (!std::filesystem::exists(root / "save.pending")) return;
    const auto staging = root / "staging";
    // Validate the complete staged dataset before copying any destination.
    readDataset(staging);
    for (const auto& file : files) {
        std::filesystem::copy_file(staging / file, root / file,
                                  std::filesystem::copy_options::overwrite_existing);
    }
    std::filesystem::remove(root / "save.pending");
}
StoreData loadStore(const std::string& directory, bool allowMissing) {
    recoverStoreSave(directory);
    const std::filesystem::path root{directory};
    std::size_t present = 0;
    for (const auto& file : files) {
        if (std::filesystem::exists(root / file) || std::filesystem::exists(root / (file + ".pending"))) ++present;
    }
    if (present == 0 && allowMissing) return {};
    if (present != files.size()) throw std::runtime_error("Missing or incomplete dataset: " + directory);
    return readDataset(root);
}
void saveStore(const StoreData& data, const std::string& directory) {
    recoverStoreSave(directory);
    const std::filesystem::path root{directory};
    const auto staging = root / "staging";
    std::filesystem::create_directories(staging);
    saveProducts(data.products, (staging / files[0]).string());
    saveCustomers(data.customers, (staging / files[1]).string());
    saveOrders(data.orders, (staging / files[2]).string(), (staging / files[3]).string());
    readDataset(staging);
    std::ofstream marker{root / "save.pending"};
    if (!marker) throw std::runtime_error("Cannot create dataset save marker.");
    marker.close();
    if (!marker) throw std::runtime_error("Cannot close dataset save marker.");
    recoverStoreSave(directory);
}
