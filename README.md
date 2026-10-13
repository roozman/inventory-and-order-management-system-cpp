# Inventory and Order Management System

A C++17 console application for managing a small store's products, stock, customers, and sales orders. The goal is to track inventory accurately, process orders without inconsistent stock changes, preserve historical prices and discounts, and save data between sessions.

This is a learning project focused on object-oriented design, standard-library containers, input validation, error handling, file persistence, and automated testing with CMake/CTest. Monetary values use integer rials.

## Current Status

The project is under development. Product and inventory operations are implemented; customers, orders, discounts, and persistence are still planned. Data currently exists only in memory and is lost when the application exits.

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
- [x] Provide automated product and inventory tests registered with CTest.

### Yet to Be Implemented

- [ ] Register and list customers, generate unique customer IDs, and find customers by ID.
- [ ] Create draft orders for existing customers with unique order IDs.
- [ ] Add and remove draft-order items with valid product references and positive quantities.
- [ ] Set and clear product discounts from 0% to 100%, applied equally to all customers.
- [ ] Calculate line and order subtotals, discounts, and final totals with integer rounding and overflow checks.
- [ ] Preserve historical product names, base prices, and discounts in order items.
- [ ] Confirm orders only when every item has sufficient stock, reducing stock exactly once without partial changes.
- [ ] Cancel draft orders without changing stock and restore confirmed-order stock exactly once on cancellation.
- [ ] Complete confirmed orders and enforce all permitted order-state transitions.
- [ ] Coordinate product archiving with open orders and stock restoration.
- [ ] Display order details, all orders, and a selected customer's order history.
- [ ] Report completed-order counts, total discounts, and net sales revenue.
- [ ] Save and load products, customers, orders, and historical amounts using a documented text format.
- [ ] Handle malformed files and save/load failures without silently losing valid data.
- [ ] Expand automated tests to cover customers, orders, discounts, reports, and persistence.
- [ ] Provide sample data, build/run/test instructions, and a complete console workflow demonstration.

## Future Direction

A Qt desktop interface will be added after the console version is complete and tested, reusing its business logic and persistence functionality.
