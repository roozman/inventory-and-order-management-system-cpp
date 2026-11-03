#include "csv_helpers.h"
#include <istream>
#include <stdexcept>

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

