cmake_minimum_required(VERSION 4.3)
project(store)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(store src/main.cpp
        src/product.h
        src/product.cpp
        src/inventory.h
        src/inventory.cpp
)

add_executable(product_tests
    tests/product_tests.cpp
        src/product.cpp
)

add_executable(inventory_tests
    tests/inventory_tests.cpp
        src/inventory.cpp
        src/product.cpp
)

target_compile_options(store PRIVATE
        -Wall -Wextra -Wpedantic
)

target_compile_options(product_tests PRIVATE
        -Wall -Wextra -Wpedantic
)
target_compile_options(inventory_tests PRIVATE
        -Wall -Wextra -Wpedantic
)

enable_testing()
add_test(NAME product_constructor_validator COMMAND product_tests valid)
add_test(NAME product_constructor_exception_empty_name COMMAND product_tests empty_name)
add_test(NAME product_constructor_exception_whitespace_name COMMAND product_tests whitespace_name)
add_test(NAME product_constructor_exception_negative_unit_price COMMAND product_tests neg_price)
add_test(NAME product_constructor_exception_negative_stock_quantity COMMAND product_tests neg_stock)
add_test(NAME product_constructor_exception_negative_reorder_level COMMAND product_tests neg_reorder)
add_test(NAME product_constructor_all_zero_values COMMAND product_tests all_zero_values)
add_test(NAME product_constructor_name_has_whitespace COMMAND product_tests name_whitespace)
add_test(NAME inventory_lookup_empty COMMAND inventory_tests inventory_lookup_empty)
add_test(NAME inventory_lookup_existing COMMAND inventory_tests inventory_lookup_existing)
add_test(NAME inventory_lookup_nonempty_unknown_sku COMMAND inventory_tests inventory_lookup_nonempty_unknown_sku)
add_test(NAME inventory_add_first_product COMMAND inventory_tests inventory_add_first_product)
add_test(NAME inventory_add_to_nonempty_vector COMMAND inventory_tests inventory_add_to_nonempty_vector)
add_test(NAME inventory_add_reject_duplicate COMMAND inventory_tests inventory_add_reject_duplicate)
add_test(NAME sku_formatting COMMAND inventory_tests sku_formatting)
add_test(NAME sku_reject_zero COMMAND inventory_tests sku_reject_zero)
add_test(NAME sku_reject_overflow COMMAND inventory_tests sku_reject_overflow)
add_test(NAME sku_next_empty COMMAND inventory_tests sku_next_empty)
add_test(NAME sku_next_unordered COMMAND inventory_tests sku_next_unordered)
add_test(NAME sku_next_exhausted COMMAND inventory_tests sku_next_exhausted)
add_test(NAME sku_consecutive_registration COMMAND inventory_tests sku_consecutive_registration)
add_test(NAME product_sku_empty COMMAND product_tests sku_empty)
add_test(NAME product_sku_too_short COMMAND product_tests sku_too_short)
add_test(NAME product_sku_too_long COMMAND product_tests sku_too_long)
add_test(NAME product_sku_wrong_prefix COMMAND product_tests sku_wrong_prefix)
add_test(NAME product_sku_invalid_digit COMMAND product_tests sku_invalid_digit)
add_test(NAME product_sku_sign COMMAND product_tests sku_sign)
add_test(NAME product_sku_whitespace COMMAND product_tests sku_whitespace)
add_test(NAME product_sku_lowercase COMMAND product_tests sku_lowercase)
add_test(NAME product_sku_reserved COMMAND product_tests sku_reserved)
add_test(NAME product_sku_maximum COMMAND product_tests sku_maximum)
