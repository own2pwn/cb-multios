#ifndef MSC_H_
#define MSC_H_

typedef struct {
    cgc_uint32_t magic;
    cgc_uint32_t tag;
    cgc_uint32_t data_length;
    cgc_uint8_t flags;
    cgc_uint8_t lun;
    cgc_uint8_t cb_length;
    cgc_uint8_t cb[0x10];
} __attribute__ ((__packed__)) cgc_cbw_t;

typedef struct {
    cgc_uint32_t magic;
    cgc_uint32_t tag;
    cgc_uint32_t residue;
    cgc_uint8_t status;
} __attribute__ ((__packed__)) cgc_csw_t;

typedef struct {
    cgc_cbw_t cbw;
    cgc_csw_t csw;
    int state;
#define MSC_ST_IDLE 0
#define MSC_ST_OUT 1
#define MSC_ST_IN 2
#define MSC_ST_STATUS 3
    int status;
    cgc_size_t count;
    cgc_uint8_t *memory;

    cgc_size_t out_length;
    const cgc_uint8_t *out_buffer;
    unsigned int out_flags;
#define MSC_F_DYNAMIC 0x01
    cgc_uint8_t out_static[4096];

    union {
        struct {
            cgc_uint32_t lba;
            cgc_uint32_t length;
        } write;
    } in_state;
} cgc_msc_t;

#include "usb.h"

int cgc_msc_init(cgc_msc_t *msc);
int cgc_msc_handle_ep0(cgc_usb_t *usb, cgc_urb_t *urb);
int cgc_msc_handle_urb(cgc_usb_t *usb, cgc_urb_t *urb);

#endif /* MSC_H_ */
