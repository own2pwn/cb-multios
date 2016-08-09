#ifndef PIZZAIOMANAGER_H_
#define PIZZAIOMANAGER_H_

#include "pizza.h"
#include "iomanager.h"

class cgc_PizzaIoManager : public cgc_IoManager
{
private:
    cgc_Pizza *pizza;

    bool cgc_create_pizza_pie();
    bool cgc_create_pizza_sub();
    bool cgc_create_pizza_bowl();

    void cgc_add_toppings();
    void cgc_remove_toppings();
    void cgc_add_sauces();
    void cgc_remove_sauces();
public:
    cgc_PizzaIoManager(cgc_Pizza *_pizza=NULL);
    virtual ~cgc_PizzaIoManager();

    bool cgc_new_pizza();
    bool cgc_edit_pizza(cgc_Pizza *_pizza=NULL);
    cgc_Pizza *cgc_get_pizza();
};

#endif
