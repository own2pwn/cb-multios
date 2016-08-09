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
#include "pizza.h"

extern "C" {
#include <string.h>
#include <stdlib.h>
}

namespace {
    /* PRIVATE HELPER FUNCTIONS */
    void cgc_print_helper(void *_topping)
    {
        cgc_Topping *cgc_topping = (cgc_Topping*)_topping;
        printf("%s", cgc_topping->cgc_get_topping_name());
        if (cgc_topping->cgc_contains_pork())
            printf("**");
        else if(!cgc_topping->cgc_is_vegetarian())
            printf("*");
        printf("\n");
    }
}


cgc_Pizza::cgc_Pizza()
{
}

cgc_Pizza::~cgc_Pizza()
{
    cgc_Topping *cgc_topping;
    cgc_Sauce *sauce;

    while (cgc_toppings.cgc_pop(cgc_topping))
        delete cgc_topping;
    while (sauce_ots.cgc_pop(sauce))
        delete sauce;
}

bool cgc_Pizza::cgc_add_topping(cgc_Topping *cgc_topping)
{
    cgc_size_t i;
    for (i = 0; i < cgc_toppings.cgc_length(); i++)
        if (*cgc_toppings[i] == *cgc_topping)
            return false;

    return cgc_toppings.cgc_add(cgc_topping);
}

bool cgc_Pizza::cgc_remove_topping(cgc_size_t idx)
{
    return cgc_toppings.cgc_remove(idx);
}

bool cgc_Pizza::cgc_add_sauce(cgc_Sauce *sauce)
{
    return sauce_ots.cgc_add(sauce);
}

bool cgc_Pizza::cgc_remove_sauce(cgc_size_t idx)
{
    return sauce_ots.cgc_remove(idx);
}

void cgc_Pizza::cgc_print_toppings()
{
    if (cgc_toppings.cgc_is_empty())
        printf("\tNone\n");
    else
        cgc_toppings.cgc_print_list(&cgc_print_helper);
}

void cgc_Pizza::cgc_print_sauces()
{
    if (sauce_ots.cgc_is_empty())
        printf("\tNone\n");
    else
        sauce_ots.cgc_print_list(&cgc_print_helper);
}

cgc_size_t cgc_Pizza::cgc_get_num_toppings()
{
    return cgc_toppings.cgc_length();
}

cgc_size_t cgc_Pizza::cgc_get_num_sauces()
{
    return sauce_ots.cgc_length();
}

void cgc_Pizza::cgc_print_pizza()
{
    printf("    Selected cgc_Toppings\n");
    cgc_print_toppings();
    printf("    cgc_Sauce on the side\n");
    cgc_print_sauces();
}


int cgc_Pizza::cgc_get_calories()
{
    cgc_size_t i, calories = 0;
    for (i = 0; i < cgc_toppings.cgc_length(); i++)
        calories += cgc_toppings[i]->cgc_get_calories();

    for (i = 0; i < sauce_ots.cgc_length(); i++)
        calories += sauce_ots[i]->cgc_get_calories();

    return calories;
}

int cgc_Pizza::cgc_get_carbs()
{
    cgc_size_t i, carbs = 0;
    for (i = 0; i < cgc_toppings.cgc_length(); i++)
        carbs += cgc_toppings[i]->cgc_get_carbs();

    for (i = 0; i < sauce_ots.cgc_length(); i++)
        carbs += sauce_ots[i]->cgc_get_carbs();

    return carbs;
}

int cgc_Pizza::cgc_get_prep_time()
{
    return cgc_toppings.cgc_length()* 30 + sauce_ots.cgc_length() * 15;
}

