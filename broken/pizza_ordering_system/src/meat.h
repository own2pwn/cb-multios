#ifndef MEAT_H_
#define MEAT_H_
#include "topping.h"

class cgc_Meat : public cgc_Topping {
private:
    bool is_pork;
    cgc_Meat(const char *_name, int _calories, int _carbs, bool _is_pork);
public:
    virtual ~cgc_Meat();
    static cgc_Meat *cgc_add_meat(const char *meat_name);
    static void cgc_list_options();
    virtual bool cgc_is_vegetarian() { return false; }
    virtual bool cgc_contains_pork() { return is_pork; }
};

#endif
