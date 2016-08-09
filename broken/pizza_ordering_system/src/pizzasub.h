#ifndef PIZZASUB_H_
#define PIZZASUB_H_
#include "pizza.h"

class cgc_PizzaSub: public cgc_Pizza {
private:
    unsigned char sub_size;
    unsigned char bread_type;

    size_t sub_calories;
    size_t sub_carbs;
public:
    cgc_PizzaSub(unsigned char _sub_size, unsigned char _bread_type);
    virtual ~cgc_PizzaSub();

    void cgc_choose_white_bread();
    void cgc_choose_wheat_bread();
    unsigned char cgc_get_sub_size();
    void cgc_set_sub_size(unsigned char _sub_size);

    virtual int cgc_get_calories();
    virtual int cgc_get_carbs();
    virtual int cgc_get_prep_time();
    virtual void cgc_print_pizza();
};

#endif
