#include "../src/storage/product_storage.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <stdexcept>

namespace {
const std::string header = "sku,name,unit_price,stock_quantity,reorder_level,discount_percent,archived\n";
const std::string row = "P001,Notebook,101,5,1,10,0\n";

void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

void writeFile(const std::string& path, const std::string& text) {
    std::ofstream file(path, std::ios::binary);
    file << text;
    file.close();
    require(bool(file), "Test fixture write failed.");
}

std::string readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    require(bool(file), "Test fixture read failed.");
    return {std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
}

void compare(const std::vector<Product>& actual, const std::vector<Product>& expected) {
    require(actual.size() == expected.size(), "Product count changed.");
    for (std::size_t i = 0; i < actual.size(); ++i) {
        const auto& a = actual[i];
        const auto& e = expected[i];
        require(a.getSku() == e.getSku() && a.getProductName() == e.getProductName() &&
                a.getUnitPrice() == e.getUnitPrice() && a.getStockQuantity() == e.getStockQuantity() &&
                a.getReorderLevel() == e.getReorderLevel() && a.getDiscount() == e.getDiscount() &&
                a.isArchived() == e.isArchived(), "Product fields changed.");
    }
}

void testRoundTrip(const std::string& path) {
    std::vector<Product> products{
        {"P001", "  Notebook, \"blue\"\nsecond\r\nthird  ", 101, 5, 1},
        {"P002", "Free", 0, 0, 0},
        {"PFFF", "Maximum", std::numeric_limits<std::int64_t>::max(),
         std::numeric_limits<int>::max(), std::numeric_limits<int>::max()}
    };
    products[0].setDiscount(10);
    products[0].archive();
    products[2].setDiscount(100);
    saveProducts(products, path);
    compare(loadProducts(path), products);
    compare(loadProducts(path), products);
    require(readFile(path) == readFile(path + ".tmp"), "Staging copy differs.");
    require(!std::filesystem::exists(path + ".pending"), "Marker remains after success.");
}

void testRepeatedSave(const std::string& path) {
    saveProducts({Product{"P001", "Old", 10, 5, 1}}, path);
    std::vector<Product> changed{{"P002", "New", 20, 2, 0}};
    saveProducts(changed, path);
    compare(loadProducts(path), changed);
    saveProducts({}, path);
    require(readFile(path) == header && loadProducts(path).empty(), "Empty save did not replace inventory.");
}

void testInvalid(const std::string& path) {
    const std::vector<std::pair<std::string, int>> fixtures{
        {"", 1}, {"wrong\n", 1}, {"\"unclosed", 1},
        {header + "P001,Name,1\n", 2},
        {header + "P001,Name,1,1,1,0,0,extra\n", 2},
        {header + "P001,\"unclosed,1,1,1,0,0", 2},
        {header + "P001,Na\"me,1,1,1,0,0", 2},
        {header + "P001,\"Name\"x,1,1,1,0,0", 2},
        {header + row + row, 3},
        {header + "P000,Name,1,1,1,0,0", 2},
        {header + "P001,\"   \",1,1,1,0,0", 2},
        {header + "P001,Name,-1,1,1,0,0", 2},
        {header + "P001,Name,123abc,1,1,0,0", 2},
        {header + "P001,Name,,1,1,0,0", 2},
        {header + "P001,Name,9223372036854775808,1,1,0,0", 2},
        {header + "P001,Name,1," + std::to_string(static_cast<long long>(std::numeric_limits<int>::max()) + 1) + ",1,0,0", 2},
        {header + "P001,Name,1,1,-1,0,0", 2},
        {header + "P001,Name,1,1,1,101,0", 2},
        {header + "P001,Name,1,1,1,0,true", 2},
        {header + row + "P002,Name,invalid,1,1,0,0", 3}
    };
    for (const auto& fixture : fixtures) {
        writeFile(path, fixture.first);
        std::vector<Product> live{{"P003", "Existing", 10, 2, 1}};
        const auto before = live;
        bool rejected = false;
        try { live = loadProducts(path); }
        catch (const std::runtime_error& error) {
            rejected = true;
            const std::string message{error.what()};
            require(message.find(path) != std::string::npos &&
                    message.find("record " + std::to_string(fixture.second)) != std::string::npos,
                    "Malformed record error lacks file/record context: " + message);
        }
        require(rejected, "Invalid CSV was accepted: " + fixture.first);
        compare(live, before);
    }
}

void testLineEndings(const std::string& path) {
    std::string crlfHeader = header.substr(0, header.size() - 1) + "\r\n";
    for (const auto& ending : {std::string{"\r\n"}, std::string{}}) {
        writeFile(path, crlfHeader + "P001,\"Notebook\",101,5,1,10,0" + ending);
        auto products = loadProducts(path);
        require(products.size() == 1 && products[0].getDiscount() == 10, "CSV line ending rejected.");
    }
}

void testRecovery(const std::string& path) {
    writeFile(path, "partial");
    writeFile(path + ".tmp", header + row);
    writeFile(path + ".pending", "");
    auto products = loadProducts(path);
    require(products.size() == 1 && products[0].getSku() == "P001", "Load did not recover.");
    require(!std::filesystem::exists(path + ".pending"), "Recovered marker remains.");
    recoverProductSave(path);
    require(readFile(path) == header + row, "Repeated recovery changed data.");
    // Without a marker, an incomplete staging write must not replace the save.
    writeFile(path + ".tmp", "unfinished");
    require(loadProducts(path).size() == 1, "Unmarked staging file was loaded.");
}

void testRecoveryFailure(const std::string& path) {
    std::filesystem::create_directory(path);
    writeFile(path + ".tmp", header + row);
    writeFile(path + ".pending", "");
    bool failed = false;
    try { saveProducts({}, path); }
    catch (const std::filesystem::filesystem_error&) { failed = true; }
    require(failed && readFile(path + ".tmp") == header + row &&
            std::filesystem::exists(path + ".pending"), "Failed recovery lost pending data.");
    std::filesystem::remove(path); // Empty fixture directory, not a save file.
    recoverProductSave(path);
    require(readFile(path) == header + row, "Retry did not restore pending data.");
}

void testMissingFiles(const std::string& path) {
    bool failed = false;
    try { loadProducts(path); }
    catch (const std::runtime_error&) { failed = true; }
    require(failed, "Missing file was silently loaded.");
    writeFile(path + ".pending", "");
    failed = false;
    try { recoverProductSave(path); }
    catch (const std::filesystem::filesystem_error&) { failed = true; }
    require(failed && std::filesystem::exists(path + ".pending"), "Missing staging file lost marker.");
}
} // namespace

int main(int argc, char* argv[]) {
    if (argc != 2) return 1;
    const std::string name{argv[1]};
    if (name != "round_trip" && name != "repeated" && name != "invalid" &&
        name != "line_endings" && name != "recovery" && name != "recovery_failure" &&
        name != "missing") return 1;
    const auto directory = std::filesystem::current_path() / ("storage_test_" + name);
    bool created = false;
    int result = 0;
    try {
        created = std::filesystem::create_directory(directory);
        require(created, "Test directory already exists; refusing to overwrite.");
        const auto path = (directory / "products.csv").string();
        if (name == "round_trip") testRoundTrip(path);
        else if (name == "repeated") testRepeatedSave(path);
        else if (name == "invalid") testInvalid(path);
        else if (name == "line_endings") testLineEndings(path);
        else if (name == "recovery") testRecovery(path);
        else if (name == "recovery_failure") testRecoveryFailure(path);
        else testMissingFiles(path);
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        result = 1;
    }
    if (created) {
        // Delete only this test's known fixtures; no recursive cleanup.
        std::error_code error;
        for (const auto& file : {"products.csv", "products.csv.tmp", "products.csv.pending"}) {
            std::filesystem::remove(directory / file, error);
        }
        std::filesystem::remove(directory, error);
    }
    return result;
}
