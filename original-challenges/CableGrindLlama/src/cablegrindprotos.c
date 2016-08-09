/*
 * Copyright (C) Narf Industries <info@narfindustries.com>
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "libc.h"
#include "cablegrind.h"
#include "cablegrindprotos.h"
#include "helpers.h"

STACKPROTECTINIT
void cgc_process_llamainutilely(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamainutilely===")-1,"===llamainutilely===");
cgc_handle_ntohs(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaantioxygen(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {cgc_process_llamasubtileness,cgc_process_llamapunaise,cgc_process_llamainutilely};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaantioxygen===")-1,"===llamaantioxygen===");
cgc_handle_ntohll(payload,size);
cgc_handle_lv_vuln(payload,size);
next = cgc_handle_nextproto_b(payload,size);
cgc_handle_string(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_byte(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_double(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_float(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_float(payload,size);
next = cgc_handle_nextproto_b(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohll(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_float(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_ntohll(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohll(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamafucate(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamafucate===")-1,"===llamafucate===");
cgc_handle_letohs(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_float(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_double(payload,size);
cgc_handle_float(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_string(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_double(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamacyanohydrin(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamacyanohydrin===")-1,"===llamacyanohydrin===");
cgc_handle_letohs(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_float(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaisonymy(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaisonymy===")-1,"===llamaisonymy===");
cgc_handle_ntohll(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_float(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_double(payload,size);
cgc_handle_float(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_string(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_double(payload,size);
cgc_handle_ntohll(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamastrawer(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {cgc_process_llamamarket};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamastrawer===")-1,"===llamastrawer===");
next = cgc_handle_nextproto_b(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_byte(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_letohs(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_lv(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_float(payload,size);
cgc_handle_lv_vuln(payload,size);
next = cgc_handle_nextproto_b(payload,size);
cgc_handle_float(payload,size);
cgc_handle_double(payload,size);
next = cgc_handle_nextproto_b(payload,size);
cgc_handle_ntohs(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamacarbonator(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamacarbonator===")-1,"===llamacarbonator===");
cgc_handle_double(payload,size);
cgc_handle_float(payload,size);
cgc_handle_string(payload,size);
cgc_handle_double(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_double(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_double(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_float(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_double(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_double(payload,size);
cgc_handle_ntohl(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamapoorness(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {cgc_process_llamasubtileness,cgc_process_llamaforebridge,cgc_process_llamaunquarrelled};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamapoorness===")-1,"===llamapoorness===");
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_float(payload,size);
cgc_handle_double(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_float(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_float(payload,size);
cgc_handle_string(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaundignifiedly(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaundignifiedly===")-1,"===llamaundignifiedly===");
cgc_handle_letohl(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_double(payload,size);
cgc_handle_lv(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaambulacriform(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {cgc_process_llamamyringotomy,cgc_process_llamaskewed};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaambulacriform===")-1,"===llamaambulacriform===");
next = cgc_handle_nextproto_b(payload,size);
cgc_handle_double(payload,size);
cgc_handle_double(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_slv(payload,size);
next = cgc_handle_nextproto_b(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_float(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_ntohll(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaJaina(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {cgc_process_llamaPantagruelistic,cgc_process_llamainutilely};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaJaina===")-1,"===llamaJaina===");
cgc_handle_string_vuln(payload,size);
cgc_handle_ntohs(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_letohl(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_lv_vuln(payload,size);
next = cgc_handle_nextproto_b(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_string_vuln(payload,size);
next = cgc_handle_nextproto_b(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_slv_vuln(payload,size);
next = cgc_handle_nextproto_b(payload,size);
next = cgc_handle_nextproto_s(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_lv(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamafolliculin(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamafolliculin===")-1,"===llamafolliculin===");
cgc_handle_ntohll(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_string(payload,size);
cgc_handle_float(payload,size);
cgc_handle_double(payload,size);
cgc_handle_double(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohs(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamadoldrums(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamadoldrums===")-1,"===llamadoldrums===");
cgc_handle_ntohl(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_double(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_string(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_double(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_float(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_double(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamatelodendron(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamatelodendron===")-1,"===llamatelodendron===");
cgc_handle_string(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_double(payload,size);
cgc_handle_double(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_double(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_float(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_double(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_ntohl(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamajibi(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {cgc_process_llamafootpad,cgc_process_llamastrawer};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamajibi===")-1,"===llamajibi===");
cgc_handle_lv_vuln(payload,size);
cgc_handle_float(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_string(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_ntohs(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamapycnite(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamapycnite===")-1,"===llamapycnite===");
cgc_handle_slv(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_lv_vuln(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaatophan(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaatophan===")-1,"===llamaatophan===");
cgc_handle_string_vuln(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_double(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_string(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_double(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_float(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_double(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_letohl(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaobtrusiveness(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaobtrusiveness===")-1,"===llamaobtrusiveness===");
cgc_handle_ntohs(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_float(payload,size);
cgc_handle_string(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_double(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_double(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_string(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamachromophotographic(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {cgc_process_llamaunacceptable,cgc_process_llamaisonymy};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamachromophotographic===")-1,"===llamachromophotographic===");
cgc_handle_string(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_string(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_double(payload,size);
cgc_handle_string(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaskewed(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaskewed===")-1,"===llamaskewed===");
cgc_handle_lv(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_double(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamapanse(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamapanse===")-1,"===llamapanse===");
cgc_handle_slv(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_double(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_float(payload,size);
cgc_handle_string(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_string(payload,size);
cgc_handle_float(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaunacceptable(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaunacceptable===")-1,"===llamaunacceptable===");
cgc_handle_lv(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_ntohll(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaPantagruelistic(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaPantagruelistic===")-1,"===llamaPantagruelistic===");
cgc_handle_float(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_float(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_double(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_double(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_lv(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaAraneiformes(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaAraneiformes===")-1,"===llamaAraneiformes===");
cgc_handle_ntohs(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_float(payload,size);
cgc_handle_double(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_double(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamascholasticly(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamascholasticly===")-1,"===llamascholasticly===");
cgc_handle_lv_vuln(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_double(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_float(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_letohl(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamamarket(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {cgc_process_llamainutilely,cgc_process_llamamyringotomy,cgc_process_llamaadulterously,cgc_process_llamaatophan};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamamarket===")-1,"===llamamarket===");
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_double(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohs(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_double(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_string_vuln(payload,size);
next = cgc_handle_nextproto_b(payload,size);
cgc_handle_double(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_float(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_float(payload,size);
cgc_handle_float(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaforebridge(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaforebridge===")-1,"===llamaforebridge===");
cgc_handle_string(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_string(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_double(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_double(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_float(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_slv_vuln(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamasporocyst(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamasporocyst===")-1,"===llamasporocyst===");
cgc_handle_slv_vuln(payload,size);
cgc_handle_double(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_string(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_float(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_double(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamahalochromy(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamahalochromy===")-1,"===llamahalochromy===");
cgc_handle_letohl(payload,size);
cgc_handle_double(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_double(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamafluctuate(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamafluctuate===")-1,"===llamafluctuate===");
cgc_handle_float(payload,size);
cgc_handle_float(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_double(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_float(payload,size);
cgc_handle_string(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_double(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_double(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_float(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_string(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_double(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_float(payload,size);
cgc_handle_lv(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamatrochocephalus(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamatrochocephalus===")-1,"===llamatrochocephalus===");
cgc_handle_letohl(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_double(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_string_vuln(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaPlautine(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaPlautine===")-1,"===llamaPlautine===");
cgc_handle_ntohl(payload,size);
cgc_handle_string(payload,size);
cgc_handle_float(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_float(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_slv_vuln(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaHoplonemertini(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaHoplonemertini===")-1,"===llamaHoplonemertini===");
cgc_handle_lv(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohs(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamapunaise(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamapunaise===")-1,"===llamapunaise===");
cgc_handle_slv(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_float(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohs(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamanatterjack(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {cgc_process_llamainutilely};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamanatterjack===")-1,"===llamanatterjack===");
cgc_handle_letohl(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_float(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_slv_vuln(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_lv(payload,size);
next = cgc_handle_nextproto_s(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamascobby(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamascobby===")-1,"===llamascobby===");
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_string(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_string(payload,size);
cgc_handle_float(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_double(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_string(payload,size);
cgc_handle_float(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_float(payload,size);
cgc_handle_double(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamahydropathy(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamahydropathy===")-1,"===llamahydropathy===");
cgc_handle_letohl(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_string(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_string(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_string(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohs(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamadentary(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamadentary===")-1,"===llamadentary===");
cgc_handle_letohs(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_string_vuln(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamadenominate(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {cgc_process_llamadermophyte,cgc_process_llamadyspneic};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamadenominate===")-1,"===llamadenominate===");
cgc_handle_letohl(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_letohs(payload,size);
next = cgc_handle_nextproto_b(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_double(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_float(payload,size);
next = cgc_handle_nextproto_b(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohs(payload,size);
next = cgc_handle_nextproto_b(payload,size);
cgc_handle_double(payload,size);
cgc_handle_ntohll(payload,size);
next = cgc_handle_nextproto_b(payload,size);
cgc_handle_double(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_string(payload,size);
next = cgc_handle_nextproto_b(payload,size);
next = cgc_handle_nextproto_b(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_float(payload,size);
next = cgc_handle_nextproto_b(payload,size);
cgc_handle_lv(payload,size);
next = cgc_handle_nextproto_s(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamadecoke(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamadecoke===")-1,"===llamadecoke===");
cgc_handle_lv(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_string(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_double(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_double(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_string(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_slv_vuln(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamadigredience(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamadigredience===")-1,"===llamadigredience===");
cgc_handle_string_vuln(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_double(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_string(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_double(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_double(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamabetitle(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamabetitle===")-1,"===llamabetitle===");
cgc_handle_ntohs(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_string(payload,size);
cgc_handle_double(payload,size);
cgc_handle_ntohs(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamatenebrose(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {cgc_process_llamascobby,cgc_process_llamaonium};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamatenebrose===")-1,"===llamatenebrose===");
cgc_handle_double(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_double(payload,size);
cgc_handle_letohll(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_lv(payload,size);
next = cgc_handle_nextproto_b(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaforgeability(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaforgeability===")-1,"===llamaforgeability===");
cgc_handle_slv_vuln(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_double(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_lv_vuln(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaunderlease(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaunderlease===")-1,"===llamaunderlease===");
cgc_handle_letohll(payload,size);
cgc_handle_double(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaunquarrelled(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaunquarrelled===")-1,"===llamaunquarrelled===");
cgc_handle_letohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_ntohl(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamasupersympathy(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamasupersympathy===")-1,"===llamasupersympathy===");
cgc_handle_ntohs(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_double(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_double(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamamegasporic(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamamegasporic===")-1,"===llamamegasporic===");
cgc_handle_letohll(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_float(payload,size);
cgc_handle_float(payload,size);
cgc_handle_string(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_double(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_float(payload,size);
cgc_handle_double(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_string(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaboll(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaboll===")-1,"===llamaboll===");
cgc_handle_byte(payload,size);
cgc_handle_string(payload,size);
cgc_handle_string(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohl(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaonium(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaonium===")-1,"===llamaonium===");
cgc_handle_double(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_string(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_float(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_string(payload,size);
cgc_handle_float(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohs(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaentodermic(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaentodermic===")-1,"===llamaentodermic===");
cgc_handle_string(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_slv(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamapolitician(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamapolitician===")-1,"===llamapolitician===");
cgc_handle_letohs(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_float(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_double(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_double(payload,size);
cgc_handle_double(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_double(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_ntohl(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamatopoalgia(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamatopoalgia===")-1,"===llamatopoalgia===");
cgc_handle_ntohll(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_double(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_ntohl(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaTamilian(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaTamilian===")-1,"===llamaTamilian===");
cgc_handle_string(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_string(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_double(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_string(payload,size);
cgc_handle_string(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_double(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_ntohl(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamasilvicultural(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamasilvicultural===")-1,"===llamasilvicultural===");
cgc_handle_float(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_string(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaadulterously(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {cgc_process_llamaatophan,cgc_process_llamaunquarrelled,cgc_process_llamaadulterously,cgc_process_llamaentodermic};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaadulterously===")-1,"===llamaadulterously===");
cgc_handle_ntohs(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_double(payload,size);
next = cgc_handle_nextproto_b(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaschoppen(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaschoppen===")-1,"===llamaschoppen===");
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_string(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_double(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_string(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_double(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_double(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_double(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_double(payload,size);
cgc_handle_string(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_ntohs(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamasart(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamasart===")-1,"===llamasart===");
cgc_handle_byte(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_double(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_float(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_double(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_double(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_double(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_float(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamamink(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamamink===")-1,"===llamamink===");
cgc_handle_byte(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaintracutaneous(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaintracutaneous===")-1,"===llamaintracutaneous===");
cgc_handle_ntohs(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_double(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_double(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_float(payload,size);
cgc_handle_double(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_double(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohll(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamafootpad(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {cgc_process_llamanatterjack};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamafootpad===")-1,"===llamafootpad===");
cgc_handle_ntohll(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_slv(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_float(payload,size);
cgc_handle_string(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_double(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_string(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_string(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_double(payload,size);
cgc_handle_double(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_double(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_float(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_lv_vuln(payload,size);
next = cgc_handle_nextproto_b(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_double(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohl(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamamyringotomy(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamamyringotomy===")-1,"===llamamyringotomy===");
cgc_handle_letohl(payload,size);
cgc_handle_ntohll(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamasubderivative(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamasubderivative===")-1,"===llamasubderivative===");
cgc_handle_ntohll(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_string(payload,size);
cgc_handle_float(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaleaflessness(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaleaflessness===")-1,"===llamaleaflessness===");
cgc_handle_string(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_string(payload,size);
next = cgc_handle_nextproto_b(payload,size);
cgc_handle_string(payload,size);
cgc_handle_letohs(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_double(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_double(payload,size);
cgc_handle_slv(payload,size);
next = cgc_handle_nextproto_s(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamasquatinid(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {cgc_process_llamasubtileness,cgc_process_llamaintracutaneous};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamasquatinid===")-1,"===llamasquatinid===");
next = cgc_handle_nextproto_b(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohs(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_float(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_float(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_float(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohll(payload,size);
next = cgc_handle_nextproto_b(payload,size);
next = cgc_handle_nextproto_s(payload,size);
next = cgc_handle_nextproto_b(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohs(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_double(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_float(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_lv_vuln(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaspaying(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaspaying===")-1,"===llamaspaying===");
cgc_handle_letohll(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_double(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohll(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaportico(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {cgc_process_llamaSoricinae};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaportico===")-1,"===llamaportico===");
cgc_handle_ntohll(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_slv_vuln(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_double(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_byte(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_byte(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaquavery(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {cgc_process_llamasubderivative};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaquavery===")-1,"===llamaquavery===");
cgc_handle_double(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_double(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_double(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_string(payload,size);
cgc_handle_lv_vuln(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_ntohl(payload,size);
next = cgc_handle_nextproto_b(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_string_vuln(payload,size);
next = cgc_handle_nextproto_b(payload,size);
cgc_handle_byte(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamagamobium(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamagamobium===")-1,"===llamagamobium===");
cgc_handle_double(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_float(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_double(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaateuchus(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaateuchus===")-1,"===llamaateuchus===");

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamadebadge(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamadebadge===")-1,"===llamadebadge===");
cgc_handle_string(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_float(payload,size);
cgc_handle_float(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_double(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_float(payload,size);
cgc_handle_double(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_ntohs(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaDonia(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaDonia===")-1,"===llamaDonia===");
cgc_handle_letohll(payload,size);
cgc_handle_float(payload,size);
cgc_handle_double(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_double(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_float(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_string(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_double(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_letohs(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamadermophyte(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamadermophyte===")-1,"===llamadermophyte===");
cgc_handle_slv(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_ntohl(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamacolophonite(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamacolophonite===")-1,"===llamacolophonite===");
cgc_handle_letohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_string(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_double(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohs(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamadyspneic(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {cgc_process_llamaentodermic,cgc_process_llamadentary,cgc_process_llamaJaina,cgc_process_llamadiscretion};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamadyspneic===")-1,"===llamadyspneic===");
cgc_handle_double(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohs(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamafurnishable(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamafurnishable===")-1,"===llamafurnishable===");

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamacombatable(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamacombatable===")-1,"===llamacombatable===");
cgc_handle_letohl(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_double(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohl(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamakirombo(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {cgc_process_llamaonium,cgc_process_llamamarket};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamakirombo===")-1,"===llamakirombo===");
cgc_handle_byte(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_double(payload,size);
cgc_handle_letohs(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_double(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_slv(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_string(payload,size);
cgc_handle_byte(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_slv(payload,size);
next = cgc_handle_nextproto_b(payload,size);
cgc_handle_string(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_string(payload,size);
cgc_handle_float(payload,size);
cgc_handle_double(payload,size);
cgc_handle_double(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_ntohl(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaworshipability(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaworshipability===")-1,"===llamaworshipability===");
cgc_handle_slv(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_float(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_double(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_double(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohs(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaparmacety(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaparmacety===")-1,"===llamaparmacety===");
cgc_handle_ntohll(payload,size);
cgc_handle_float(payload,size);
cgc_handle_string(payload,size);
cgc_handle_float(payload,size);
cgc_handle_double(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_double(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamapronephros(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamapronephros===")-1,"===llamapronephros===");
cgc_handle_slv(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_double(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_string(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_float(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_double(payload,size);
cgc_handle_double(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_ntohll(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamasmolt(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamasmolt===")-1,"===llamasmolt===");
cgc_handle_slv_vuln(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_double(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_double(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_float(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_double(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_string(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_string(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaJuletta(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {cgc_process_llamaunquarrelled,cgc_process_llamaaalii,cgc_process_llamatrochocephalus};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaJuletta===")-1,"===llamaJuletta===");
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohs(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_string(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_float(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_double(payload,size);
cgc_handle_string_vuln(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_slv_vuln(payload,size);
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_double(payload,size);
next = cgc_handle_nextproto_b(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamalutecia(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamalutecia===")-1,"===llamalutecia===");
cgc_handle_float(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_lv_vuln(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaposit(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaposit===")-1,"===llamaposit===");
cgc_handle_ntohs(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_double(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_double(payload,size);
cgc_handle_string(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_string_vuln(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamacarnaptious(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamacarnaptious===")-1,"===llamacarnaptious===");
cgc_handle_ntohll(payload,size);
cgc_handle_string(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_string(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_string(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_string_vuln(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaferocity(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaferocity===")-1,"===llamaferocity===");
cgc_handle_letohs(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_slv_vuln(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamacoruscant(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamacoruscant===")-1,"===llamacoruscant===");
cgc_handle_slv_vuln(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_double(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_double(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_string(payload,size);
cgc_handle_lv_vuln(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaaalii(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaaalii===")-1,"===llamaaalii===");
cgc_handle_lv_vuln(payload,size);
cgc_handle_double(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_float(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_string(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_string_vuln(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaParthenon(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaParthenon===")-1,"===llamaParthenon===");
cgc_handle_byte(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_string(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_float(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamasubtileness(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamasubtileness===")-1,"===llamasubtileness===");
cgc_handle_letohll(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_double(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_double(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_double(payload,size);
cgc_handle_string(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_slv_vuln(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamahemostat(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamahemostat===")-1,"===llamahemostat===");
cgc_handle_lv(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_float(payload,size);
cgc_handle_float(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamadiscretion(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamadiscretion===")-1,"===llamadiscretion===");
cgc_handle_lv_vuln(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_letohs(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamamitigate(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamamitigate===")-1,"===llamamitigate===");
cgc_handle_slv(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamamachar(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamamachar===")-1,"===llamamachar===");
cgc_handle_double(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_double(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_double(payload,size);
cgc_handle_float(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohll(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamafactorization(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamafactorization===")-1,"===llamafactorization===");
cgc_handle_float(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_lv(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaantimaterialistic(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaantimaterialistic===")-1,"===llamaantimaterialistic===");
cgc_handle_string(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_string(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_ntohl(payload,size);
cgc_handle_double(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_lv_vuln(payload,size);
cgc_handle_string(payload,size);
cgc_handle_string(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamalowermost(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamalowermost===")-1,"===llamalowermost===");
cgc_handle_byte(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_string(payload,size);
cgc_handle_float(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_ntohll(payload,size);
cgc_handle_string_vuln(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_letohs(payload,size);
cgc_handle_letohll(payload,size);
cgc_handle_letohl(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_float(payload,size);
cgc_handle_double(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamaSoricinae(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamaSoricinae===")-1,"===llamaSoricinae===");
cgc_handle_slv_vuln(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}
STACKPROTECTINIT
void cgc_process_llamamononomial(uint8_t **payload, int *size) {
void (*next_dissectors[]) (uint8_t **, int *) = {cgc_process_llamahalochromy};
STACKPROTECTADD
int next = -1;
SSENDL(sizeof("===llamamononomial===")-1,"===llamamononomial===");
next = cgc_handle_nextproto_s(payload,size);
cgc_handle_string(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_lv(payload,size);
cgc_handle_byte(payload,size);
cgc_handle_slv_vuln(payload,size);
cgc_handle_slv(payload,size);
cgc_handle_ntohs(payload,size);
cgc_handle_ntohs(payload,size);

STACKPROTECTCHK
if(*size > 0 && next >= 0 && next < sizeof(next_dissectors)/sizeof(next_dissectors[0])) next_dissectors[next](payload,size);
}

