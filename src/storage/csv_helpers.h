#ifndef STORE_CSV_HELPERS_H
#define STORE_CSV_HELPERS_H

#include <iosfwd>
#include <string>
#include <vector>

// Quotes one field and doubles embedded double quotes.
std::string escapeCsvField(const std::string& text);

// Reads one logical record, preserving quoted line breaks. Returns false at EOF.
// Clears fields before reading; throws std::runtime_error for malformed CSV or I/O failure.
bool readCsvRecord(std::istream& input, std::vector<std::string>& fields);

#endif //STORE_CSV_HELPERS_H
