#ifndef ORDERIOMANAGER_H_
#define ORDERIOMANAGER_H_

#include "order.h"
#include "iomanager.h"

class cgc_OrderIoManager : public cgc_IoManager
{
private:
    cgc_List<cgc_Order> orders;
    void cgc_list_orders(); //Lists order + order name
public:
    cgc_OrderIoManager();
    virtual ~cgc_OrderIoManager();
    bool cgc_input_order(cgc_size_t idx=-1);
    bool cgc_update_order();
    bool cgc_delete_order();
    void cgc_clear_all_orders(bool force=false);
    bool cgc_view_single_order();
    void cgc_print_orders(); //cgc_List order, pizza info, and everything else
};

#endif
