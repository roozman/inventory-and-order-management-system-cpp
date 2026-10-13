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
        src/customer.h
        src/customer.cpp
        src/customer_registry.cpp
        src/customer_registry.h
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

add_executable(customer_tests
    tests/customer_tests.cpp
    src/customer.cpp
    src/customer_registry.cpp
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
target_compile_options(customer_tests PRIVATE
        -Wall -Wextra -Wpedantic
)

enable_testing()
add_test(NAME customer_valid COMMAND customer_tests valid)
add_test(NAME customer_invalid_ids COMMAND customer_tests invalid_ids)
add_test(NAME customer_invalid_names COMMAND customer_tests invalid_names)
add_test(NAME customer_lookup_empty COMMAND customer_tests lookup_empty)
add_test(NAME customer_lookup_existing COMMAND customer_tests lookup_existing)
add_test(NAME customer_lookup_missing COMMAND customer_tests lookup_missing)
add_test(NAME customer_add COMMAND customer_tests add)
add_test(NAME customer_add_duplicate COMMAND customer_tests add_duplicate)
add_test(NAME customer_id_formatting COMMAND customer_tests id_formatting)
add_test(NAME customer_id_invalid_range COMMAND customer_tests id_invalid_range)
add_test(NAME customer_next_id_empty COMMAND customer_tests next_id_empty)
add_test(NAME customer_next_id_unordered COMMAND customer_tests next_id_unordered)
add_test(NAME customer_next_id_exhausted COMMAND customer_tests next_id_exhausted)
add_test(NAME customer_consecutive_registration COMMAND customer_tests consecutive_registration)
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
add_test(NAME product_set_name_valid COMMAND product_tests set_name_valid)
add_test(NAME product_set_name_with_spaces COMMAND product_tests set_name_with_spaces)
add_test(NAME product_set_name_reject_empty COMMAND product_tests set_name_empty)
add_test(NAME product_set_name_reject_whitespace COMMAND product_tests set_name_whitespace)
add_test(NAME product_set_price_positive COMMAND product_tests set_price_positive)
add_test(NAME product_set_price_zero COMMAND product_tests set_price_zero)
add_test(NAME product_set_price_large_int64 COMMAND product_tests set_price_large)
add_test(NAME product_set_price_reject_negative COMMAND product_tests set_price_negative)
add_test(NAME product_set_reorder_positive COMMAND product_tests set_reorder_positive)
add_test(NAME product_set_reorder_zero COMMAND product_tests set_reorder_zero)
add_test(NAME product_set_reorder_reject_negative COMMAND product_tests set_reorder_negative)
add_test(NAME product_increase_stock_positive COMMAND product_tests increase_stock_positive)
add_test(NAME product_increase_stock_reject_zero COMMAND product_tests increase_stock_zero)
add_test(NAME product_increase_stock_reject_negative COMMAND product_tests increase_stock_negative)
add_test(NAME product_increase_stock_to_int_max COMMAND product_tests increase_stock_to_max)
add_test(NAME product_increase_stock_reject_overflow COMMAND product_tests increase_stock_overflow)
add_test(NAME product_decrease_stock_positive COMMAND product_tests decrease_stock_positive)
add_test(NAME product_decrease_stock_reject_zero COMMAND product_tests decrease_stock_zero)
add_test(NAME product_decrease_stock_reject_negative COMMAND product_tests decrease_stock_negative)
add_test(NAME product_decrease_stock_to_zero COMMAND product_tests decrease_stock_to_zero)
add_test(NAME product_decrease_stock_reject_excessive COMMAND product_tests decrease_stock_excessive)
add_test(NAME product_low_stock_below_threshold COMMAND product_tests low_stock_below)
add_test(NAME product_low_stock_equal_threshold COMMAND product_tests low_stock_equal)
add_test(NAME product_low_stock_above_threshold COMMAND product_tests low_stock_above)
add_test(NAME product_low_stock_zero_threshold COMMAND product_tests low_stock_zero_threshold)
add_test(NAME inventory_low_stock_empty COMMAND inventory_tests low_stock_empty_inventory)
add_test(NAME inventory_low_stock_no_matches COMMAND inventory_tests low_stock_no_matches)
add_test(NAME inventory_low_stock_one_match COMMAND inventory_tests low_stock_one_match)
add_test(NAME inventory_low_stock_mixed COMMAND inventory_tests low_stock_mixed_inventory)
add_test(NAME inventory_low_stock_zero_threshold COMMAND inventory_tests low_stock_zero_threshold)
add_test(NAME product_archive_new_product_active COMMAND product_tests archive_new_product_active)
add_test(NAME product_archive_changes_state COMMAND product_tests archive_changes_state)
add_test(NAME product_archive_reject_repeated COMMAND product_tests archive_reject_repeated)
add_test(NAME product_archive_reject_name_edit COMMAND product_tests archive_reject_name_edit)
add_test(NAME product_archive_reject_price_edit COMMAND product_tests archive_reject_price_edit)
add_test(NAME product_archive_reject_reorder_edit COMMAND product_tests archive_reject_reorder_edit)
add_test(NAME product_archive_reject_stock_increase COMMAND product_tests archive_reject_stock_increase)
add_test(NAME product_archive_reject_stock_decrease COMMAND product_tests archive_reject_stock_decrease)
add_test(NAME inventory_archived_product_remains_findable COMMAND inventory_tests archived_product_remains_findable)
add_test(NAME inventory_archived_sku_remains_reserved COMMAND inventory_tests archived_sku_remains_reserved)
add_test(NAME inventory_archived_product_excluded_from_low_stock COMMAND inventory_tests archived_product_excluded_from_low_stock)
add_test(NAME inventory_archiving_one_does_not_affect_another COMMAND inventory_tests archiving_one_product_does_not_affect_another)
