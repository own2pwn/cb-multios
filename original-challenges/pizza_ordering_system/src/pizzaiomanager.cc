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
#include "pizzaiomanager.h"
#include "pizzapie.h"
#include "pizzabowl.h"
#include "pizzasub.h"
#include "sauce.h"
#include "cheese.h"
#include "meat.h"
#include "veggie.h"

cgc_PizzaIoManager::cgc_PizzaIoManager(cgc_Pizza *_pizza)
{
    pizza = _pizza;
}

cgc_PizzaIoManager::~cgc_PizzaIoManager()
{
    pizza = NULL;
}

bool cgc_PizzaIoManager::cgc_create_pizza_pie()
{
    int choice;
    while (true) {
        printf("Select Size\n");
        printf("1. Small\n");
        printf("2. Medium\n");
        printf("3. Large\n");
        printf("Choice: ");

        choice = cgc_readnum();
        if (choice < 1 || choice > 3) {
            printf("Bad Selection\n");
            continue;
        }
        break;
    }

    pizza = new cgc_PizzaPie(choice);
    if (!pizza) {
        printf("Error creating a pizza right now. Please try again later\n");
        return false;
    }
    printf("Successfully added a new cgc_Pizza Pie!\n");
    return true;
}

bool cgc_PizzaIoManager::cgc_create_pizza_sub()
{
    int sub_size, bread_type, choice;
    while (true) {
        printf("Select Size\n");
        printf("1. 6 inch\n");
        printf("2. 12 inch\n");
        printf("Choice: ");

        choice = cgc_readnum();
        if (choice < 1 || choice > 2) {
            printf("Bad Selection\n");
            continue;
        }
        sub_size = choice;
        break;
    }

    while (true) {
        printf("Select Bread Type\n");
        printf("1. White\n");
        printf("2. Wheat\n");
        printf("Choice: ");

        choice = cgc_readnum();
        if (choice < 1 || choice > 2) {
            printf("Bad Selection\n");
            continue;
        }
        bread_type = choice;
        break;
    }

    pizza = new cgc_PizzaSub(sub_size, bread_type);
    if (!pizza) {
        printf("Error creating a pizza right now. Please try again later\n");
        return false;
    }
    printf("Successfully added a new cgc_Pizza Sub!\n");
    return true;
}

bool cgc_PizzaIoManager::cgc_create_pizza_bowl()
{
    int choice;
    bool side_of_bread;
    while (true) {
        printf("Side of bread?\n");
        printf("1. Yes\n");
        printf("2. No\n");
        printf("Choice: ");

        choice = cgc_readnum();
        switch (choice) {
        case 1:
            side_of_bread = true; break;
        case 2:
            side_of_bread = false; break;
        default:
            printf("Bad Selection\n");
            continue;
        }
        break;
    }

    pizza = new cgc_PizzaBowl(side_of_bread);
    if (!pizza) {
        printf("Error creating a pizza right now. Please try again later\n");
        return false;
    }
    printf("Successfully added a new cgc_Pizza Bowl!\n");
    return true;
}

void cgc_PizzaIoManager::cgc_add_toppings()
{
    cgc_Topping *topping = NULL;
    int choice;
    bool done_adding_toppings = false;

    while(!done_adding_toppings) {
        printf("Select topping type:\n");
        printf("1. cgc_Cheese\n");
        printf("2. cgc_Meat\n");
        printf("3. Veggies\n");
        printf("4. No More Toppings\n");
        printf("Choice: ");

        choice = cgc_readnum();
        switch(choice) {
        case 1:
            cgc_Cheese::cgc_list_options();
            printf("Enter topping name: ");
            if(cgc_readline())
                topping = cgc_Cheese::cgc_select_cheese(cgc_get_last_input());
            else
                printf("Failed reading input\n");
            break;
        case 2:
            cgc_Meat::cgc_list_options();
            printf("Enter topping name: ");
            if(cgc_readline())
                topping = cgc_Meat::cgc_add_meat(cgc_get_last_input());
            else
                printf("Failed reading input\n");
            break;
        case 3:
            cgc_Veggie::cgc_list_options();
            printf("Enter topping name: ");
            if(cgc_readline())
                topping = cgc_Veggie::cgc_pick_veggie(cgc_get_last_input());
            else
                printf("Failed reading input\n");
            break;
        case 4:
            done_adding_toppings = true;
            continue;
        default:
            printf("Bad Choice\n");
            continue;
        }

        if (topping) {
            pizza->cgc_add_topping(topping);
            printf("Added topping\n");
        } else {
            printf("Bad topping name\n");
        }
    }
}

void cgc_PizzaIoManager::cgc_remove_toppings()
{
    int choice;
    bool done_removing_toppings = false;

    while (!done_removing_toppings) {
        if (!pizza->cgc_get_num_toppings()) {
            printf("No Toppings to cgc_remove\n");
            return;
        }

        printf("Toppings Added\n");
        printf("\t0. Cancel\n");
        pizza->cgc_print_toppings();
        printf("Choice: ");
        choice = cgc_readnum();

        if (choice == 0) {
            printf("Finished removing toppings\n");
            return;
        } else if (choice > pizza->cgc_get_num_toppings()) {
            printf("Bad Selection\n");
        } else {
            printf("Removed Topping\n");
            pizza->cgc_remove_topping(--choice);
        }
    }
}

void cgc_PizzaIoManager::cgc_add_sauces()
{
    cgc_Sauce *sauce = NULL;
    int choice;
    bool done_adding_sauces = false;

    while(!done_adding_sauces) {
        printf("Select an option:\n");
        printf("1. Add cgc_Sauce\n");
        printf("2. No More Sauces\n");
        printf("Choice: ");

        choice = cgc_readnum();
        switch(choice) {
        case 1:
            cgc_Sauce::cgc_list_options();
            printf("Enter sauce name: ");
            if(cgc_readline())
                sauce = cgc_Sauce::cgc_pour_sauce(cgc_get_last_input());
            else
                printf("Failed reading input\n");
            break;
        case 2:
            done_adding_sauces = true; continue;
        default:
            printf("Bad Choice\n");
            continue;
        }

        if (sauce) {
            pizza->cgc_add_sauce(sauce);
            printf("Added sauce\n");
        } else {
            printf("Bad sauce name\n");
        }
    }
}

void cgc_PizzaIoManager::cgc_remove_sauces()
{
    int choice;
    bool done_removing_sauces = false;

    while (!done_removing_sauces) {
        if (!pizza->cgc_get_num_sauces()) {
            printf("No sauces to cgc_remove\n");
            return;
        }

        printf("Sauces on the side\n");
        printf("\t0. Cancel\n");
        pizza->cgc_print_sauces();
        printf("Choice: ");
        choice = cgc_readnum();

        if (choice == 0) {
            printf("Finished removing sauces\n");
            return;
        } else if (choice > pizza->cgc_get_num_sauces()) {
            printf("Bad Selection\n");
        } else {
            printf("Removed sauce\n");
            pizza->cgc_remove_sauce(--choice);
        }
    }
}

bool cgc_PizzaIoManager::cgc_new_pizza()
{
    int choice;

    while (true) {
        printf("Choose what the kind of pizza\n");
        printf("1. cgc_Pizza Pie - The classic!\n");
        printf("2. cgc_Pizza Sub - All the fun, on a bun\n");
        printf("3. cgc_Pizza Bowl - Our own twist\n");
        printf("Choice: ");
        choice = cgc_readnum();
        switch(choice) {
        case 1:
            cgc_create_pizza_pie(); break;
        case 2:
            cgc_create_pizza_sub(); break;
        case 3:
            cgc_create_pizza_bowl(); break;
        default:
            printf("Bad Choice\n");
            continue;
        }

        break;
    }

    if (cgc_edit_pizza()) {
        printf("Successfully added pizza!\n");
        return true;
    } else {
        printf("Error: Try again later\n");
        return false;
    }
}

bool cgc_PizzaIoManager::cgc_edit_pizza(cgc_Pizza *_pizza)
{
    if (_pizza)
        pizza = _pizza;

    if (!pizza) {
        printf("No cgc_Pizza to edit\n");
        return false;
    }

    int choice;
    bool done_editing = false;
    while (!done_editing) {
        printf("Select an option:\n");
        printf("1. Add Toppings\n");
        printf("2. Remove Toppings\n");
        printf("3. Add cgc_Sauce\n");
        printf("4. Remove cgc_Sauce\n");
        printf("5. Finished With cgc_Pizza\n");
        printf("Choice: ");
        choice = cgc_readnum();
        switch(choice) {
        case 1:
            cgc_add_toppings(); break;
        case 2:
            cgc_remove_toppings(); break;
        case 3:
            cgc_add_sauces(); break;
        case 4:
            cgc_remove_sauces(); break;
        case 5:
            done_editing = true; break;
        default:
            printf("Bad Choice\n");
            continue;
        }
    }

    return true;
}

cgc_Pizza *cgc_PizzaIoManager::cgc_get_pizza()
{
    cgc_Pizza *ppizza = pizza;
    pizza = NULL;
    return ppizza;
}
