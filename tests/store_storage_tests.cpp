#include "../src/storage/store_storage.h"
#include "../src/storage/customer_storage.h"
#include "../src/storage/storage_helpers.h"
#include "../src/product/inventory.h"
#include "../src/customer/customer_registry.h"
#include "../src/order/order_registry.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <functional>
#include <stdexcept>

namespace {
const std::vector<std::string> orderHeader{
    "order_id","customer_id","status","item_count","subtotal","discount_amount","final_total"};
const std::vector<std::string> itemHeader{
    "order_id","sku","product_name","unit_price","quantity","discount_percent","subtotal","discount_amount","final_total"};
void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}
void fails(const std::function<void()>& operation, const std::string& context = "") {
    try { operation(); }
    catch (const std::exception& error) {
        require(context.empty() || std::string{error.what()}.find(context) != std::string::npos,
                "Error lacks expected context: " + std::string{error.what()});
        return;
    }
    throw std::runtime_error("Expected operation to fail.");
}
std::string read(const std::filesystem::path& path) {
    std::ifstream file{path, std::ios::binary};
    require(bool(file), "Cannot read fixture.");
    return {std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
}
void write(const std::filesystem::path& path, const std::string& text) {
    std::ofstream file{path, std::ios::binary};
    file << text;
    file.close();
    require(bool(file), "Cannot write fixture.");
}
StoreData fixture() {
    StoreData data;
    data.products.emplace_back("P001", "Old, \"name\"\r\nsecond", 101, 30, 2);
    data.products.emplace_back("P002", "Pen", 50, 10, 1);
    data.products[0].setDiscount(10);
    data.customers.emplace_back("C001", " Alice, \"A\"\nSmith ");
    data.customers.emplace_back("C002", "Bob");
    for (int i = 1; i <= 4; ++i) {
        data.orders.emplace_back(generateOrderID(i), i % 2 == 0 ? "C002" : "C001");
        data.orders.back().addItem(data.products[0], i);
        data.orders.back().addItem(data.products[1], 1);
    }
    data.orders[1].confirm(data.products);
    data.orders[2].confirm(data.products);
    data.orders[2].complete();
    data.orders[3].confirm(data.products);
    data.orders[3].cancelConfirmed(data.products);
    data.orders.emplace_back("O000005", "C001");
    data.products[0].setProductName("Current name");
    data.products[0].setUnitPrice(999);
    data.products[0].setDiscount(50);
    data.products[0].archive();
    return data;
}
void compare(const StoreData& a, const StoreData& b) {
    require(a.products.size() == b.products.size() && a.customers.size() == b.customers.size() &&
            a.orders.size() == b.orders.size(), "Collection sizes changed.");
    for (std::size_t i = 0; i < a.products.size(); ++i) {
        const auto& x = a.products[i]; const auto& y = b.products[i];
        require(x.getSku() == y.getSku() && x.getProductName() == y.getProductName() &&
            x.getUnitPrice() == y.getUnitPrice() && x.getStockQuantity() == y.getStockQuantity() &&
            x.getReorderLevel() == y.getReorderLevel() && x.getDiscount() == y.getDiscount() &&
            x.isArchived() == y.isArchived(), "Product round trip changed fields.");
    }
    for (std::size_t i = 0; i < a.customers.size(); ++i)
        require(a.customers[i].getId() == b.customers[i].getId() &&
            a.customers[i].getName() == b.customers[i].getName(), "Customer fields changed.");
    for (std::size_t i = 0; i < a.orders.size(); ++i) {
        const auto& x = a.orders[i]; const auto& y = b.orders[i];
        require(x.getId() == y.getId() && x.getCustomerId() == y.getCustomerId() &&
            x.getOrderStatus() == y.getOrderStatus() && x.getItems().size() == y.getItems().size() &&
            x.getSubTotal() == y.getSubTotal() && x.getDiscountAmount() == y.getDiscountAmount() &&
            x.getFinalTotal() == y.getFinalTotal(), "Order fields changed.");
        for (std::size_t j = 0; j < x.getItems().size(); ++j) {
            const auto& p = x.getItems()[j]; const auto& q = y.getItems()[j];
            require(p.getProductSKU() == q.getProductSKU() && p.getProductName() == q.getProductName() &&
                p.getProductUnitPrice() == q.getProductUnitPrice() && p.getOrderQuantity() == q.getOrderQuantity() &&
                p.getDiscount() == q.getDiscount() && p.getTotalPrice() == q.getTotalPrice() &&
                p.getDiscountAmount() == q.getDiscountAmount() && p.getFinalTotal() == q.getFinalTotal(),
                "Historical item fields changed.");
        }
    }
}
void testReopen(const std::filesystem::path& dir) {
    const auto original = fixture();
    saveStore(original, dir.string());
    auto loaded = loadStore(dir.string());
    compare(loaded, original);
    loaded = loadStore(dir.string());
    compare(loaded, original);
    const int before = loaded.products[0].getStockQuantity();
    loaded.orders[1].cancelConfirmed(loaded.products);
    require(loaded.products[0].getStockQuantity() == before + 2 &&
        loaded.products[0].isArchived(), "Restored cancellation did not restore archived stock.");
    fails([&] { loaded.orders[1].cancelConfirmed(loaded.products); });
    require(loaded.products[0].getStockQuantity() == before + 2, "Repeated cancellation changed stock.");
    addProduct(loaded.products, Product{generateSKU(findNextID(loaded.products)), "New", 1, 1, 0});
    addCustomer(loaded.customers, Customer{generateCustomerId(findNextCustomerId(loaded.customers)), "New"});
    addDraftOrder(loaded.customers, loaded.orders, loaded.customers.back().getId());
    require(loaded.products.back().getSku() == "P003" && loaded.customers.back().getId() == "C003" &&
        loaded.orders.back().getId() == "O000006", "Identifiers were reused after loading.");
    saveStore(loaded, dir.string());
    compare(loadStore(dir.string()), loaded);
}
void testCustomers(const std::filesystem::path& dir) {
    const auto path = (dir / "customers.csv").string();
    const auto data = fixture();
    saveCustomers(data.customers, path);
    const auto loaded = loadCustomers(path);
    require(loaded.size() == 2 && loaded[0].getName() == data.customers[0].getName(), "Customer escaping failed.");
    const std::vector<std::string> bad{
        "", "bad\n", "customer_id,name\nC000,Alice\n", "customer_id,name\nC001,\"  \"\n",
        "customer_id,name\nC001,Alice\nC001,Bob\n", "customer_id,name\nC001\n",
        "customer_id,name\nC001,\"unfinished"};
    for (const auto& text : bad) {
        write(path, text);
        fails([&] { loadCustomers(path); }, "record");
    }
    saveCustomers({}, path);
    require(loadCustomers(path).empty(), "Empty customer file failed.");
}
void testInvalidOrders(const std::filesystem::path& dir) {
    const auto data = fixture();
    saveStore(data, dir.string());
    const auto path = (dir / "orders.csv").string();
    const auto original = readCsvTable(path, orderHeader);
    const std::vector<std::pair<std::size_t, std::string>> bad{
        {0,"O000000"},{1,"CFFF"},{2,"Shipped"},{3,"99"},{3,"-1"},{4,"0"},{5,"0"},
        {6,"0"},{4,"9223372036854775808"}};
    for (const auto& change : bad) {
        auto rows = original;
        rows[0][change.first] = change.second;
        writeCsvTable(path, orderHeader, rows);
        auto live = data;
        fails([&] { live = loadStore(dir.string()); }, "orders.csv, record");
        compare(live, data);
    }
    auto rows = original;
    rows.push_back(rows[0]);
    writeCsvTable(path, orderHeader, rows);
    fails([&] { loadStore(dir.string()); }, "Duplicate");
    rows = original; rows[4][2] = "Confirmed";
    writeCsvTable(path, orderHeader, rows);
    fails([&] { loadStore(dir.string()); }, "require items");
    write(path, "bad header\n");
    fails([&] { loadStore(dir.string()); }, "record 1");
}
void testInvalidItems(const std::filesystem::path& dir) {
    saveStore(fixture(), dir.string());
    const auto path = (dir / "order_items.csv").string();
    const auto original = readCsvTable(path, itemHeader);
    const std::vector<std::pair<std::size_t, std::string>> bad{
        {0,"O999999"},{1,"PFFF"},{2," "},{3,"-1"},{3,"9223372036854775807"},
        {4,"0"},{4,"2147483648"},{5,"101"},{6,"0"},{7,"0"},{8,"0"}};
    for (const auto& change : bad) {
        auto rows = original;
        rows[0][change.first] = change.second;
        writeCsvTable(path, itemHeader, rows);
        fails([&] { loadStore(dir.string()); }, "order_items.csv, record");
    }
    auto rows = original;
    rows.push_back(rows[0]);
    writeCsvTable(path, itemHeader, rows);
    fails([&] { loadStore(dir.string()); }, "Duplicate");
    rows = original; rows.erase(rows.begin());
    writeCsvTable(path, itemHeader, rows);
    fails([&] { loadStore(dir.string()); }, "item count");
    rows = original; rows[0].pop_back();
    writeCsvTable(path, itemHeader, rows);
    fails([&] { loadStore(dir.string()); }, "field count");
}
void testRecovery(const std::filesystem::path& dir) {
    auto data = fixture();
    saveStore(data, dir.string());
    // Force failure after products have copied but before customers can copy.
    std::filesystem::remove(dir / "customers.csv");
    std::filesystem::create_directory(dir / "customers.csv");
    data.products[1].setUnitPrice(75);
    fails([&] { saveStore(data, dir.string()); });
    require(std::filesystem::exists(dir / "save.pending"), "Failed copy lost dataset marker.");
    const auto pending = read(dir / "staging" / "products.csv");
    fails([&] { saveStore({}, dir.string()); });
    require(read(dir / "staging" / "products.csv") == pending, "Retry destroyed pending data.");
    std::filesystem::remove(dir / "customers.csv");
    compare(loadStore(dir.string()), data);
    require(!std::filesystem::exists(dir / "save.pending"), "Recovery did not clear marker.");
    compare(loadStore(dir.string()), data);
}
void testFailedSave(const std::filesystem::path& dir) {
    auto data = fixture();
    saveStore(data, dir.string());
    const auto original = data;
    data.customers.clear(); // Invalid references must be detected before publishing.
    fails([&] { saveStore(data, dir.string()); }, "Missing customer");
    compare(loadStore(dir.string()), original);
    require(!std::filesystem::exists(dir / "save.pending"), "Invalid staging was marked complete.");
}
void testMissing(const std::filesystem::path& dir) {
    require(loadStore(dir.string(), true).products.empty(), "First startup was not empty.");
    fails([&] { loadStore(dir.string()); });
    write(dir / "products.csv", "bad");
    fails([&] { loadStore(dir.string(), true); }, "incomplete");
    std::filesystem::remove(dir / "products.csv");
    saveStore({}, dir.string());
    const auto empty = loadStore(dir.string());
    require(empty.products.empty() && empty.customers.empty() && empty.orders.empty(), "Empty dataset failed.");
    write(dir / "save.pending", "");
    std::filesystem::remove(dir / "staging" / "orders.csv");
    fails([&] { loadStore(dir.string()); });
    require(std::filesystem::exists(dir / "save.pending"), "Broken staging lost marker.");
}
void cleanup(const std::filesystem::path& dir) {
    // Only known fixture names in directories this process created; no recursive deletion.
    std::error_code error;
    for (const auto& folder : {dir / "staging", dir}) {
        for (const auto& file : {"products.csv", "customers.csv", "orders.csv", "order_items.csv"}) {
            for (const auto& suffix : {"", ".tmp", ".pending"})
                std::filesystem::remove(folder / (std::string{file} + suffix), error);
        }
        std::filesystem::remove(folder / "save.pending", error);
        std::filesystem::remove(folder, error);
    }
}
}
int main(int argc, char* argv[]) {
    if (argc != 2) return 1;
    const std::string name{argv[1]};
    if (name != "reopen" && name != "customers" && name != "invalid_orders" &&
        name != "invalid_items" && name != "recovery" && name != "failed_save" && name != "missing") return 1;
    const auto dir = std::filesystem::current_path() / ("store_storage_test_" + name);
    bool created = false;
    int result = 0;
    try {
        created = std::filesystem::create_directory(dir);
        require(created, "Test directory exists; refusing to overwrite.");
        if (name == "reopen") testReopen(dir);
        else if (name == "customers") testCustomers(dir);
        else if (name == "invalid_orders") testInvalidOrders(dir);
        else if (name == "invalid_items") testInvalidItems(dir);
        else if (name == "recovery") testRecovery(dir);
        else if (name == "failed_save") testFailedSave(dir);
        else testMissing(dir);
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        result = 1;
    }
    if (created) cleanup(dir);
    return result;
}
