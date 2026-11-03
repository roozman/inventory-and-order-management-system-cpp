#ifndef STORE_STORAGE_HELPERS_H
#define STORE_STORAGE_HELPERS_H
#include <cstdint>
#include <string>
#include <vector>
using CsvRows = std::vector<std::vector<std::string>>;
CsvRows readCsvTable(const std::string& path, const std::vector<std::string>& header);
void writeCsvTable(const std::string& path, const std::vector<std::string>& header, const CsvRows& rows);
std::int64_t parseStorageNumber(const std::string& text, std::int64_t maximum);
void recoverFileSave(const std::string& path);
#endif
