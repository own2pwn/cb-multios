#include "msc.h"

#ifndef USB_H_
#define USB_H_

#define VERSION_CODE 0x0111

typedef struct {
    cgc_uint8_t intf_class;
    cgc_uint8_t intf_subclass;
    cgc_uint8_t intf_proto;
    cgc_uint8_t padding;
} cgc_intf_t;

typedef struct {
    char path[256];
    char busid[32];
    cgc_uint32_t busnum;
    cgc_uint32_t devnum;
    cgc_uint32_t speed;
    cgc_uint16_t vendor;
    cgc_uint16_t product;
    cgc_uint16_t device;
    cgc_uint8_t dev_class;
    cgc_uint8_t dev_subclass;
    cgc_uint8_t dev_proto;
    cgc_uint8_t current_config;
    cgc_uint8_t num_configs;
    cgc_uint8_t num_intf;
    cgc_intf_t interfaces[0];
} cgc_devinfo_t;

/* metadata commands */
typedef struct {
    cgc_uint16_t version;
    cgc_uint16_t command;
#define DEVLIST_REQ 0x8008
#define DEVLIST_REP 0x0008
#define IMPORT_REQ 0x8009
#define IMPORT_REP 0x0009
    cgc_uint32_t status;
} cgc_metadata_hdr_t;

typedef struct {
    cgc_metadata_hdr_t hdr;
    cgc_uint32_t length;
    cgc_devinfo_t devices[0];
} cgc_devlist_rep_t;

typedef struct {
    char busid[32];
} cgc_import_req_t;

typedef struct {
    cgc_metadata_hdr_t hdr;
    cgc_devinfo_t device;
} cgc_import_rep_t;

/* data commands */
typedef struct {
    cgc_uint32_t command;
#define SUBMIT_REQ 0x00000001
#define SUBMIT_REP 0x00000003
#define UNLINK_REQ 0x00000002
#define UNLINK_REP 0x00000004
    cgc_uint32_t seqnum;
    cgc_uint32_t devid;
    cgc_uint32_t direction;
#define DIR_OUT 0
#define DIR_IN 1
    cgc_uint32_t endpoint;
} cgc_data_hdr_t;

typedef struct {
    cgc_uint32_t flags;
    cgc_uint32_t length;
    cgc_uint32_t start_frame;
    cgc_uint32_t number_of_packets;
    cgc_uint32_t interval;
    cgc_uint8_t setup[8];
} cgc_submit_req_t;

typedef struct {
    cgc_data_hdr_t hdr;
    cgc_uint32_t status;
    cgc_uint32_t length;
    cgc_uint32_t start_frame;
    cgc_uint32_t number_of_packets;
    cgc_uint32_t error_length;
    cgc_uint8_t setup[8];
} cgc_submit_rep_t;

typedef struct {
    cgc_uint32_t seqnum;
} cgc_unlink_req_t;

typedef struct {
    cgc_data_hdr_t hdr;
    cgc_uint32_t status;
} cgc_unlink_rep_t;

typedef struct {
    cgc_uint32_t devid;
    cgc_uint32_t seqnum;
    cgc_uint32_t direction;
    cgc_uint32_t endpoint;
    cgc_uint32_t flags;
    cgc_uint32_t length;
    cgc_uint32_t interval;
    cgc_uint8_t setup[8];
    cgc_uint8_t data[0] __attribute__((__aligned__(16)));
} cgc_urb_t;

typedef struct {
    size_t (*recv) (void *buf, size_t len);
    size_t (*send) (const void *buf, size_t len);

    int attached;
    cgc_msc_t msc;
} cgc_usb_t;

typedef struct {
    cgc_uint8_t bLength;
    cgc_uint8_t bDescriptorType;
    cgc_uint16_t bcdUSB;
    cgc_uint8_t bDeviceClass;
    cgc_uint8_t bDeviceSubClass;
    cgc_uint8_t bDeviceProtocol;
    cgc_uint8_t bMaxPacketSize0;
    cgc_uint16_t idVendor;
    cgc_uint16_t idProduct;
    cgc_uint16_t bcdDevice;
    cgc_uint8_t iManufacturer;
    cgc_uint8_t iProduct;
    cgc_uint8_t iSerialNumber;
    cgc_uint8_t bNumConfigurations;
} __attribute__((__packed__)) cgc_device_desc_t;

typedef struct {
    cgc_uint8_t bLength;
    cgc_uint8_t bDescriptorType;
    cgc_uint16_t wTotalLength;
    cgc_uint8_t bNumInterfaces;
    cgc_uint8_t bConfigurationValue;
    cgc_uint8_t iConfiguration;
    cgc_uint8_t bmAttributes;
    cgc_uint8_t bMaxPower;
} __attribute__((__packed__)) cgc_config_desc_t;

typedef struct {
    cgc_uint8_t bLength;
    cgc_uint8_t bDescriptorType;
    cgc_uint8_t bInterfaceNumber;
    cgc_uint8_t bAlternateSetting;
    cgc_uint8_t bNumEndpoints;
    cgc_uint8_t bInterfaceClass;
    cgc_uint8_t bInterfaceSubClass;
    cgc_uint8_t bInterfaceProtocol;
    cgc_uint8_t iInterface;
} __attribute__((__packed__)) cgc_intf_desc_t;

typedef struct {
    cgc_uint8_t bLength;
    cgc_uint8_t bDescriptorType;
    cgc_uint8_t bEndpointAddress;
    cgc_uint8_t bmAttributes;
    cgc_uint16_t wMaxPacketSize;
    cgc_uint8_t bInterval;
} __attribute__((__packed__)) cgc_ep_desc_t;

int cgc_usb_init(cgc_usb_t *self);
int cgc_usb_handle_packet(cgc_usb_t *self);
int cgc_usb_send_reply(cgc_usb_t *self, cgc_urb_t *urb, cgc_uint32_t status, cgc_uint32_t length);
#endif /* USB_H_ */
