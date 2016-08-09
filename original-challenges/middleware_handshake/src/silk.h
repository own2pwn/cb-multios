#ifndef SILK_H_
#define SILK_H_

#include <stdlib.h>
#include <stdint.h>

typedef struct {
    void *priv;
} cgc_silk_t;

typedef struct {
    cgc_uint32_t codes;
} __attribute__((packed)) cgc_silk_negotiate_t;

typedef struct {
    cgc_uint16_t plen;
    cgc_uint16_t glen;
    cgc_uint16_t qlen;
    cgc_uint16_t alen;
    cgc_uint8_t data[];
} __attribute__((packed)) cgc_silk_kx_param_t;

typedef struct {
    cgc_uint16_t blen;
    cgc_uint8_t data[];
} __attribute__((packed)) cgc_silk_kx_reply_t;

typedef struct {
    cgc_uint16_t datalen;
    cgc_uint8_t data[];
} __attribute__((packed)) cgc_silk_data_t;

enum {
    PKT_ERROR,
    PKT_NEGOTIATE,
    PKT_KX_PARAM,
    PKT_KX_REPLY,
    PKT_DATA
};

int cgc_silk_init(cgc_silk_t *silk);

// high-level API
int cgc_silk_prepare(cgc_silk_t *silk);
int cgc_silk_send(cgc_silk_t *silk, const unsigned char *data, unsigned int cnt);
int cgc_silk_recv(cgc_silk_t *silk, unsigned char *out, unsigned int cnt);

// low-level API
int cgc_silk_negotiate(cgc_silk_t *silk);
int cgc_silk_kx(cgc_silk_t *silk);

#endif
