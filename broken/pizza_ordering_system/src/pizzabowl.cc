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
#include "pizzabowl.h"
#define BOWL_PREP_TIME (120)

extern "C" {
#include <stdlib.h>
}

#define BREAD_CALORIES 180
#define BREAD_CARBS 26

cgc_PizzaBowl::cgc_PizzaBowl(bool _side_of_bread)
{
    side_of_bread = _side_of_bread;
}

cgc_PizzaBowl::~cgc_PizzaBowl()
{
}

void cgc_PizzaBowl::cgc_add_side_of_bread()
{
    side_of_bread = true;
}

void cgc_PizzaBowl::cgc_remove_side_of_bread()
{
    side_of_bread = false;
}

int cgc_PizzaBowl::cgc_get_calories()
{
    size_t bread_calories = side_of_bread ? BREAD_CALORIES : 0;
    return cgc_Pizza::cgc_get_calories() + bread_calories;
}


int cgc_PizzaBowl::cgc_get_carbs()
{
    size_t bread_carbs = side_of_bread ? BREAD_CARBS : 0;
    return cgc_Pizza::cgc_get_carbs() + bread_carbs;
}

int cgc_PizzaBowl::cgc_get_prep_time()
{
    return cgc_Pizza::cgc_get_prep_time() + BOWL_PREP_TIME;
}

void cgc_PizzaBowl::cgc_print_pizza()
{
    const char *bread_side = "WHOOPS";
    if (side_of_bread)
        bread_side = "Yep";
    else
        bread_side = "Nah";

    printf("The Famous cgc_Pizza Bowl. Bread on the side? - %s\n", bread_side);
    cgc_Pizza::cgc_print_pizza();
}
