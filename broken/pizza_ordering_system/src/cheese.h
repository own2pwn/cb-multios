#ifndef CHEESE_H_
#define CHEESE_H_
#include "topping.h"

class cgc_Cheese : public cgc_Topping {
private:
    cgc_Cheese(const char *_name, int _calories, int _carbs);
public:
    virtual ~cgc_Cheese();
    static cgc_Cheese *cgc_select_cheese(const char *cheese_name);
    static void cgc_list_options();
    virtual bool cgc_is_vegetarian() { return true; }
    virtual bool cgc_contains_pork() { return false; }
};

#endif
