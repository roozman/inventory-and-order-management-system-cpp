# Inventory and Order Management System

A C++17 console application for managing a small store's products, stock, customers, and sales orders. The goal is to track inventory accurately, process orders without inconsistent stock changes, preserve historical prices and discounts, and save data between sessions.

This is a learning project focused on object-oriented design, standard-library containers, input validation, error handling, file persistence, and automated testing with CMake/CTest. Monetary values use integer rials.

## Current Status

The project is under development. Product, inventory, and customer operations are available through the console. The core supports creating Draft orders and adding/removing items, preserving historical names and prices, and calculating totals with overflow checks. The console can look up and display an order, but creating and editing orders through the menu is still pending.

Order confirmation, cancellation, completion, product discounts, sales reporting, and persistence remain to be implemented. Discounts currently return zero, so the final total equals the subtotal. Data exists only in memory and is lost when the application exits.

The current milestone is task 30: order calculations and display. Next is validating confirmation conditions without changing stock.

## Progress Checklist

### Implemented

- [x] C++17 project structure with separate headers and source files and a CMake build.
- [x] Console menus for product registration, listing, and product operations.
- [x] Store multiple products in memory.
- [x] Generate unique SKUs automatically and reject duplicate SKUs.
- [x] Find products by SKU.
- [x] Validate product names, prices, stock quantities, and reorder levels.
- [x] Edit product names, unit prices, and reorder levels.
- [x] Increase and decrease stock with checks against overflow and insufficient stock.
- [x] Display active products whose stock is at or below their reorder level.
- [x] Archive products, retain their SKUs, and prevent further product edits or manual stock changes.
- [x] Validate numeric product input and report operation errors through the console.
- [x] Register and list customers, generate unique customer IDs, and find customers by ID through the console.
- [x] Create Draft orders for existing customers with generated unique order IDs through core functions.
- [x] Add existing products with positive quantities and remove items through core functions, without changing stock; reject repeated SKUs within an order.
- [x] Preserve historical product names and base prices in order items.
- [x] Calculate line and order subtotals with overflow checks; expose zero discount and the final total, including zero totals for an empty Draft.
- [x] Find an order by ID through the console and display its customer ID, status, historical item details, and totals.
- [x] Provide automated product, inventory, customer, Draft-item, and order-total tests registered with CTest. The last full run passed all 120 tests.

### Yet to Be Implemented

- [ ] Connect Draft creation and item addition/removal to the console menu.
- [ ] Set and clear product discounts from 0% to 100%, applied equally to all customers.
- [ ] Calculate actual line discounts and discounted order totals with integer rounding and overflow checks.
- [ ] Extend historical item snapshots to preserve discount percentages and discounted amounts.
- [ ] Confirm orders only when every item has sufficient stock, reducing stock exactly once without partial changes.
- [ ] Cancel draft orders without changing stock and restore confirmed-order stock exactly once on cancellation.
- [ ] Complete confirmed orders and enforce all permitted order-state transitions.
- [ ] Coordinate product archiving with open orders and stock restoration.
- [ ] List all orders and display a selected customer's order history.
- [ ] Report completed-order counts, total discounts, and net sales revenue.
- [ ] Save and load products, customers, orders, and historical amounts using a documented text format.
- [ ] Handle malformed files and save/load failures without silently losing valid data.
- [ ] Expand automated tests to cover order transitions, rejection of item edits outside Draft status, discounts, reports, and persistence.
- [ ] Provide loadable sample data files, build/run/test instructions, and a complete console workflow demonstration.

## Temporary Startup Data

`createTestingData()` currently generates five products, five customers, and two Draft orders at every launch. Each order contains two products; adding these Draft items does not reserve or reduce stock. This is temporary in-memory demonstration data, not saved data.

## Future Direction

A Qt desktop interface will be added after the console version is complete and tested, reusing its business logic and persistence functionality.
