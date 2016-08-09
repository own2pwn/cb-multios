#ifndef PIZZA_H_
#define PIZZA_H_

extern "C"
{
#include <libcgc.h>
};

#include "list.h"
#include "topping.h"
#include "sauce.h"

enum { SMALL=1, MED=2, LG=3, SIXINCH=1, TWELVEINCH=2 };
enum { WHITE=1, WHEAT=2 };

class cgc_Pizza {
private:
    cgc_List<cgc_Topping *> cgc_toppings;
    cgc_List<cgc_Sauce *> sauce_ots; //ots = On the side
public:
    cgc_Pizza();
    virtual ~cgc_Pizza();
    bool cgc_add_topping(cgc_Topping *topping);
    bool cgc_remove_topping(size_t idx);
    bool cgc_add_sauce(cgc_Sauce *sauce);
    bool cgc_remove_sauce(size_t idx);
    void cgc_print_toppings();
    void cgc_print_sauces();
    size_t cgc_get_num_toppings();
    size_t cgc_get_num_sauces();
    virtual void cgc_print_pizza();

    virtual int cgc_get_calories() = 0;
    virtual int cgc_get_carbs() = 0;
    virtual int cgc_get_prep_time() = 0;
};

#endif
