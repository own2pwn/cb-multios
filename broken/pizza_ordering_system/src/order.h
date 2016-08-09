#ifndef ORDER_H_
#define ORDER_H_

#include "pizza.h"
#include "list.h"

class cgc_Order
{
public:
    static const size_t NAME_SIZE = 32;
private:
    char pickup_name[NAME_SIZE];
    cgc_List<cgc_Pizza *> pizzas;
public:
    cgc_Order();
    ~cgc_Order();
    int cgc_wait_time();
    char *cgc_get_name();
    bool cgc_set_name(char *name);
    bool cgc_add_pizza(cgc_Pizza *pizza);
    void cgc_remove_pizza(size_t idx);
    void cgc_remove_all_pizzas();
    cgc_Pizza *cgc_get_pizza(size_t idx);
    int cgc_get_num_pizzas();
    void cgc_print_order();
};

#endif
