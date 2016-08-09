/*
 * Copyright (c) 2015 Kaprica Security, Inc.
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#include "orderiomanager.h"
#include "pizzaiomanager.h"

cgc_OrderIoManager::cgc_OrderIoManager()
{
}

cgc_OrderIoManager::~cgc_OrderIoManager()
{
}

void cgc_OrderIoManager::cgc_list_orders()
{
    int i = 0;
    for (i = 0; i < orders.cgc_length(); i++) {
        printf("%d - %s: Ordered %d pizza(s)\n", i+1, orders[i].cgc_get_name(), orders[i].cgc_get_num_pizzas());
    }
}

bool cgc_OrderIoManager::cgc_input_order(size_t idx)
{
    if (orders.cgc_is_full()) {
        printf("Maxed out orders\n");
        return false;
    }

    cgc_Order order;
    cgc_PizzaIoManager pim;
    cgc_Pizza *pizza;
    int choice;
    bool done_adding_pizzas = false;
    bool is_new_order = true;

    if (idx == -1) {
        printf("Enter Pickup Name: ");
        while (!cgc_readline(cgc_Order::NAME_SIZE)) {
            printf("Bad Pickup Name\n");
            printf("Enter Pickup Name: ");
        }

        if (!order.cgc_set_name(cgc_get_last_input()))
            return false;

        pim.cgc_new_pizza();
        pizza = pim.cgc_get_pizza();
        if (pizza)
            order.cgc_add_pizza(pizza);
    } else {
        is_new_order = false;
        order = orders[idx];
        printf("Editing order for %s\n", order.cgc_get_name());
    }

    while (!done_adding_pizzas) {
        printf("1. Add another cgc_Pizza\n");
        printf("2. Quit\n");
        if (order.cgc_get_num_pizzas() > 1)
            printf("3. Remove cgc_Pizza from order\n");

        while (true) {
            printf("Choice: ");
            choice = cgc_readnum();
            if (choice == 1) {
                pim.cgc_new_pizza();
                pizza = pim.cgc_get_pizza();
                if (pizza)
                    order.cgc_add_pizza(pizza);
                break;
            } else if (choice == 2) {
                done_adding_pizzas = true;
                break;
#ifdef PATCHED
            } else if (choice == 3 && order.cgc_get_num_pizzas() > 1) {
#else
            } else if (choice == 3) {
#endif
                printf("0. Cancel\n");
                order.cgc_print_order();
                printf("Choice: ");
                choice = cgc_readnum();
                if (choice == 0) {
                    printf("Canceled\n");
                    break;
                } else if (choice > order.cgc_get_num_pizzas()) {
                    printf("Bad Selection\n");
                } else {
                    order.cgc_remove_pizza(--choice);
                    printf("Removed Item #%d\n", choice+1);
                }
                break;
            } else {
                printf("Bad Selection\n");
            }
        }
    }

    if (is_new_order) {
        orders.cgc_add(order);
        printf("cgc_Order successfully added!\n");
    } else {
        orders[idx] = order;
        printf("cgc_Order successfully updated!\n");
    }
    return true;
}

bool cgc_OrderIoManager::cgc_update_order()
{
    cgc_Order my_order;
    int choice;

    if (orders.cgc_is_empty()) {
        printf("No orders have been inputted\n");
        return false;
    }

    printf("Update an cgc_Order\n");
    printf("0. Cancel\n");
    cgc_list_orders();
    printf("Choice: ");
    choice = cgc_readnum();
    if (choice == 0) {
        printf("Canceled\n");
        return false;
    } else if (choice > orders.cgc_length()) {
        printf("Bad Selection\n");
        return false;
    }

    return cgc_input_order(--choice);
}

bool cgc_OrderIoManager::cgc_delete_order()
{
    cgc_Order my_order;
    int choice;

    if (orders.cgc_is_empty()) {
        printf("No orders have been inputted\n");
        return false;
    }

    printf("Delete an cgc_Order\n");
    printf("0. Cancel\n");
    cgc_list_orders();
    printf("Choice: ");
    choice = cgc_readnum();
    if (choice == 0) {
        printf("Canceled\n");
        return false;
    } else if (choice < 1 || choice > orders.cgc_length()) {
        printf("Bad Selection\n");
        return false;
    }

    orders[--choice].cgc_remove_all_pizzas();
    return orders.cgc_remove(choice);
}

void cgc_OrderIoManager::cgc_clear_all_orders(bool force)
{
    cgc_Order order;
    int choice;

    if (!force) {
        printf("Delete ALL Orders\n");
        printf("0. Cancel\n");
        printf("1. Confirm\n");
        printf("Choice: ");
        choice = cgc_readnum();
        if (choice == 0) {
            printf("Canceled\n");
            return;
        } else if (choice > 1) {
            printf("Bad Selection\n");
        }

        printf("Removing all orders\n");
    }

    while(orders.cgc_pop(order)) {
        order.cgc_remove_all_pizzas();
    }
}


bool cgc_OrderIoManager::cgc_view_single_order()
{
    cgc_Order my_order;
    int choice, i;

    if (orders.cgc_is_empty()) {
        printf("No orders have been inputted\n");
        return false;
    }

    printf("View single cgc_Order\n");
    printf("0. Cancel\n");
    cgc_list_orders();
    printf("Choice: ");
    choice = cgc_readnum();
    if (choice == 0) {
        printf("Canceled\n");
        return false;
    } else if (choice > orders.cgc_length()) {
        printf("Bad Selection\n");
        return false;
    }

    i = --choice;
    printf("%d - %s: Ordered %d pizza(s)\n", i+1, orders[i].cgc_get_name(), orders[i].cgc_get_num_pizzas());
    orders[i].cgc_print_order();
    printf("*  = Contains Meat\n");
    printf("** = Contains Pork\n");
    return true;
}

void cgc_OrderIoManager::cgc_print_orders()
{
    int i = 0;

    if (orders.cgc_is_empty()) {
        printf("No orders have been inputted\n");
        return;
    }

    for (i = 0; i < orders.cgc_length(); i++) {
        printf("%d - %s: Ordered %d pizza(s)\n", i+1, orders[i].cgc_get_name(), orders[i].cgc_get_num_pizzas());
        orders[i].cgc_print_order();
    }
    printf("*  = Contains Meat\n");
    printf("** = Contains Pork\n");
}
