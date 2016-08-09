#ifndef PROTO_H
#define PROTO_H
#include "stdint.h" 
#include "libcgc.h"
#include "libc.h"
#define ERRQUIT "Mkay"
#define SENDM(b,s) if(cgc_sendmsg(STDOUT,b,s)<0) _terminate(23);
#define RECVM(b,s,ss) if(cgc_recvmsg(STDIN,b,s,ss)<0){\
     cgc_create_resp_pkt(curpkt,ERRQUIT);\
     SENDD(curpkt,curpkt->hdr.size);\
    _terminate(24);}
#define RECVD() if(cgc_recvdgram(STDIN)<0){\
     cgc_create_resp_pkt(curpkt,ERRQUIT);\
     SENDD(curpkt,curpkt->hdr.size);\
    _terminate(25);}
#define SENDD(m,s) if(cgc_senddgram(STDOUT,m,s)<0) _terminate(26);
#define MAX_FRAG 256
#define MAX_DATA_LEN 256+128
#define MAX_FRAG_DATA_LEN (MAX_DATA_LEN-8)
#define MAX_PKT_SIZE MAX_FRAG_DATA_LEN*MAX_FRAG
#define TFTTP_STRUCT_SIZE sizeof(cgc_tfttp_hdr)+MAX_PKT_SIZE
#define MAX_RETRANS 5

enum pkttype {
    PUT,
    GET,
    RAND,
    RESP,
    FRAG,
    UNKNOWN
};

typedef struct cgc_tfttp_hdr {
    size_t size; //size of trailing data
    enum pkttype code;
} cgc_tfttp_hdr;

typedef struct cgc_tfttp_pkt {
    cgc_tfttp_hdr hdr;
    uint8_t data[0];
} cgc_tfttp_pkt;

typedef struct cgc_tfttp_frag {
    uint8_t seqid;
    uint8_t numfrag;
    uint16_t unused;
    enum pkttype code;
    uint8_t data[MAX_FRAG_DATA_LEN]; //6 less for the header
} cgc_tfttp_frag;

cgc_tfttp_pkt *curpkt;

int cgc_sendmsg(int fd, uint8_t *data, size_t size); 
int cgc_recvmsg(int fd, uint8_t *data, size_t maxsize, size_t minsize);
int cgc_recvdgram(int fd);
int cgc_senddgram(int fd, cgc_tfttp_pkt *msg, size_t size);
void cgc_create_resp_pkt(cgc_tfttp_pkt* pkt, char *resp);
#endif
