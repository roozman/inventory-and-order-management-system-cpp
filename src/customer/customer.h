#ifndef STORE_CUSTOMER_H
#define STORE_CUSTOMER_H
#include <string>

class Customer {
private:
    std::string id_;
    std::string name_;

    // Accepts names containing at least one non-whitespace character.
    static bool customerNameValidator(const std::string& name);


public:
    // Requires an ID from C001 to CFFF and a non-blank name; throws std::invalid_argument otherwise.
    Customer(std::string id, std::string name);
    std::string getId() const;
    std::string getName() const;
};
#endif //STORE_CUSTOMER_H
