#ifndef SAUCE_H_
#define SAUCE_H_
#include "topping.h"

class cgc_Sauce : public cgc_Topping {
private:
    cgc_Sauce(const char *_name, int _calories, int _carbs);
public:
    virtual ~cgc_Sauce();
    static cgc_Sauce *cgc_pour_sauce(const char *sauce_name);
    static void cgc_list_options();
    virtual bool cgc_is_vegetarian() { return true; }
    virtual bool cgc_contains_pork() { return false; }
};

#endif
