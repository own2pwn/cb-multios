#ifndef VEGGIE_H_
#define VEGGIE_H_
#include "topping.h"

class cgc_Veggie : public cgc_Topping
{
private:
    cgc_Veggie(const char *_name, int _calories, int _carbs);
public:
    virtual ~cgc_Veggie();
    static cgc_Veggie *cgc_pick_veggie(const char *veggie_name);
    static void cgc_list_options();
    virtual bool cgc_is_vegetarian() { return true; }
    virtual bool cgc_contains_pork() { return false; }
};

#endif
