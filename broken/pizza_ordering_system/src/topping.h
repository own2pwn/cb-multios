#ifndef TOPPING_H_
#define TOPPING_H_

class cgc_Topping
{
protected:
    const char *name;
    int calories;
    int carbs;

public:
    virtual ~cgc_Topping();
    const char *cgc_get_topping_name() { return name; }
    int cgc_get_calories() { return calories; }
    int cgc_get_carbs() { return carbs; }

    bool operator==(const cgc_Topping &rhs) const;
    bool operator!=(const cgc_Topping &rhs) const;
    virtual bool cgc_is_vegetarian() = 0;
    virtual bool cgc_contains_pork() = 0;
};

#endif
