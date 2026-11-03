# Inventory and Order Management System

A C++17 console application for managing products, inventory, customers, and orders, with CSV persistence and automated tests.

- Manage stock, archive products, and apply product discounts.
- Create, confirm, cancel, and complete orders.
- Preserve historical item names, prices, and discounts.
- View low-stock products, customer history, and completed-sales reports.

Money uses integer rials. Discounts range from 0–100% and round down per line. Confirmation deducts stock; cancelling a confirmed order restores it once. Only completed orders count as sales.

## Build and test

Requires CMake 4.3+, a C++17 compiler, and a compatible build tool. Tested with MinGW on Windows; Linux execution has not yet been verified.

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Run `./build/store` on Linux or `.\build\store.exe` in PowerShell. With a multi-configuration generator, build with `--config Debug`, test with `-C Debug`, and use the executable in `build/Debug/`.

## Usage and sample data

Run from the repository root to keep saves in its `data/` directory. Main menu options **9**, **10**, and **11** save, load, and save-and-exit. Loading asks before replacing unsaved changes.

Sample data is included in `data/` and loads automatically when you run from the repository root. No file copying is needed. It contains five products, three customers, and five orders across all statuses. The initial sales summary shows one completed order and **36000 rials** in revenue. Saving updates these files, so your demo changes will appear in Git.

## Persistence

`products.csv`, `customers.csv`, `orders.csv`, and `order_items.csv` form one dataset. Headers define the fields; IDs connect records. Names use CSV quoting, and order items retain historical values.

Startup begins with an empty store only when the entire dataset is absent. Invalid or incomplete datasets are rejected without replacing live data. Saves use staging files and a pending marker to retry failed copies. Keep these recovery files intact. The application supports one writer at a time; saving is not guaranteed against power loss.

## Structure

`src/product/`, `src/customer/`, and `src/order/` contain business logic; `src/storage/` handles persistence; `src/main.cpp` contains the console interface. Tests are in `tests/`.

A Qt interface is planned, reusing the existing business logic and storage.
