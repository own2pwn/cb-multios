#ifndef ENGINE_H_
#define ENGINE_H_

#include <stdint.h>

enum {
    WHITE,
    BLACK
};

enum {
    WHITE_WON,
    BLACK_WON,
    DRAW,
    UNFINISHED
};

enum {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    NUM_PIECES = KING + 1,
    EMPTY = NUM_PIECES
};

typedef cgc_uint64_t cgc_bboard_t;
typedef cgc_uint8_t cgc_piece_t;
#define PIECE_MASK 0x0f
#define COLOR_SHIFT 7
#define PIECE_COLOR(p) ((cgc_piece_t)(p) >> COLOR_SHIFT)
typedef struct {
    cgc_int8_t sr, sc, dr, dc;
    cgc_uint8_t p, op, cap, prio;
} cgc_move_t;

void cgc_engine_destroy();
int cgc_engine_get_cgc_random();
void cgc_engine_go();
int cgc_engine_init();
void cgc_engine_move(cgc_move_t move);
void cgc_engine_new();
void cgc_engine_offer_draw();
void cgc_engine_result();
void cgc_engine_set_color(int);
void cgc_engine_set_depth(int);
void cgc_engine_set_go(int);
void cgc_engine_set_cgc_random(int);
void cgc_engine_undo();

#endif
