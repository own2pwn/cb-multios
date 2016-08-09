#ifndef PIZZAPIE_H_
#define PIZZAPIE_H_
#include "pizza.h"

class cgc_PizzaPie : public cgc_Pizza {
private:
    unsigned char pie_size;
    cgc_size_t pie_calories;
    cgc_size_t pie_carbs;
public:
    cgc_PizzaPie(unsigned char _pie_size);
    virtual ~cgc_PizzaPie();

    unsigned char cgc_get_pie_size();
    void cgc_set_pie_size(unsigned char _pie_size);

    virtual int cgc_get_calories();
    virtual int cgc_get_carbs();
    virtual int cgc_get_prep_time();
    virtual void cgc_print_pizza();
};

#endif
