#include "storage_helpers.h"
#include "csv_helpers.h"
#include <filesystem>
#include <fstream>
#include <stdexcept>

void recoverFileSave(const std::string& path) {
    if (!std::filesystem::exists(path + ".pending")) return;
    std::filesystem::copy_file(path + ".tmp", path, std::filesystem::copy_options::overwrite_existing);
    std::filesystem::remove(path + ".pending");
}
std::int64_t parseStorageNumber(const std::string& text, std::int64_t maximum) {
    if (text.empty() || text.find_first_not_of("0123456789") != std::string::npos)
        throw std::invalid_argument("Invalid nonnegative decimal integer: " + text);
    const auto value = std::stoll(text);
    if (value > maximum) throw std::out_of_range("Numeric field exceeds maximum.");
    return value;
}
CsvRows readCsvTable(const std::string& path, const std::vector<std::string>& header) {
    recoverFileSave(path);
    std::ifstream file{path, std::ios::binary};
    if (!file) throw std::runtime_error("Cannot open file: " + path);
    CsvRows rows;
    std::vector<std::string> fields;
    std::size_t record{1};
    try {
        if (!readCsvRecord(file, fields) || fields != header)
            throw std::runtime_error("Invalid or missing header.");
        while (true) {
            ++record;
            if (!readCsvRecord(file, fields)) break;
            if (fields.size() != header.size()) throw std::runtime_error("Wrong field count.");
            rows.push_back(fields);
        }
    } catch (const std::exception& error) {
        throw std::runtime_error(path + ", record " + std::to_string(record) + ": " + error.what());
    }
    return rows;
}
void writeCsvTable(const std::string& path, const std::vector<std::string>& header, const CsvRows& rows) {
    recoverFileSave(path);
    std::ofstream file{path + ".tmp", std::ios::binary};
    if (!file) throw std::runtime_error("Cannot open staging file: " + path);
    const auto writeRow = [&file](const std::vector<std::string>& fields) {
        for (std::size_t i = 0; i < fields.size(); ++i) {
            if (i != 0) file << ',';
            file << escapeCsvField(fields[i]);
        }
        file << '\n';
    };
    writeRow(header);
    for (const auto& row : rows) writeRow(row);
    file.close();
    if (!file) throw std::runtime_error("Failed to write staging file: " + path);
    std::ofstream marker{path + ".pending"};
    if (!marker) throw std::runtime_error("Cannot create pending marker: " + path);
    marker.close();
    if (!marker) throw std::runtime_error("Cannot close pending marker: " + path);
    recoverFileSave(path);
}
