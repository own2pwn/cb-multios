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
extern "C" {
#include <string.h>
#include <stdlib.h>
};

#include "order.h"

cgc_Order::cgc_Order()
{
}

cgc_Order::~cgc_Order()
{
}

int cgc_Order::cgc_wait_time()
{
    size_t i = 0, cgc_wait_time = 0, wait_time_modifier = 1, prep_time = 0;
    size_t max_wait_time = pizzas[0]->cgc_get_prep_time();
    for (i = 0; i < pizzas.cgc_length(); i++) {
        prep_time = pizzas[i]->cgc_get_prep_time();
        cgc_wait_time += prep_time;

        if (max_wait_time < prep_time)
            max_wait_time = prep_time;
    }

    //Factor in wait time grows with larger orders, but not linearly
    wait_time_modifier += pizzas.cgc_length() / 5;
    printf("cgc_Pizza cgc_length... = %d\n", pizzas.cgc_length());
    return (cgc_wait_time + ((max_wait_time * wait_time_modifier) / pizzas.cgc_length()));
}

char *cgc_Order::cgc_get_name()
{
    return pickup_name;
}

bool cgc_Order::cgc_set_name(char *name)
{
    if (cgc_strlen(name) >= NAME_SIZE)
        return false;

    cgc_strcpy(pickup_name, name);
    return true;
}

bool cgc_Order::cgc_add_pizza(cgc_Pizza *pizza)
{
    return pizzas.cgc_add(pizza);
}

void cgc_Order::cgc_remove_pizza(size_t idx)
{
    cgc_Pizza *pizza = pizzas[idx];
    pizzas.cgc_remove(idx);
    delete pizza;
}

void cgc_Order::cgc_remove_all_pizzas()
{
    cgc_Pizza *pizza;

    while (pizzas.cgc_pop(pizza)) {
        delete pizza;
    }
}

cgc_Pizza *cgc_Order::cgc_get_pizza(size_t idx)
{
    if (idx >= pizzas.cgc_length())
        return NULL;

    return pizzas[idx];
}

int cgc_Order::cgc_get_num_pizzas()
{
    return pizzas.cgc_length();
}

void cgc_Order::cgc_print_order()
{
    int i, calories = 0, carbs = 0;
    printf("==================================================\n");
    for (i = 0; i < pizzas.cgc_length(); i++) {
        printf("  Item #%d. ", i+1);
        pizzas[i]->cgc_print_pizza();
        calories += pizzas[i]->cgc_get_calories();
        carbs += pizzas[i]->cgc_get_carbs();
    }
    printf("--------------------------------------\n");
    printf("\t\tCalories: %d\n", calories);
    printf("\t\tCarbs   : %d\n\n", carbs);
    printf("\t\tEstimated wait time: %d minute(s)\n", cgc_wait_time()/60);
    printf("==================================================\n");
}

