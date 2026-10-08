cmake_minimum_required(VERSION 4.3)
project(store)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(store main.cpp
        product.h
        product.cpp
)

add_executable(product_tests
    tests/product_tests.cpp
        product.cpp
)

target_compile_options(store PRIVATE
        -Wall -Wextra -Wpedantic
)

target_compile_options(product_tests PRIVATE
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