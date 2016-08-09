#ifndef PIZZABOWL_H_
#define PIZZABOWL_H_
#include "pizza.h"

class cgc_PizzaBowl: public cgc_Pizza {
private:
    bool side_of_bread;

public:
    cgc_PizzaBowl(bool _side_of_bread = false);
    virtual ~cgc_PizzaBowl();

    void cgc_add_side_of_bread();
    void cgc_remove_side_of_bread();

    virtual int cgc_get_calories();
    virtual int cgc_get_carbs();
    virtual int cgc_get_prep_time();
    virtual void cgc_print_pizza();
};

#endif
