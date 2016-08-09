/*
 * Copyright (c) 2015 Kaprica Security, Inc.
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <libcgc.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef cgc_int8_t cgc_s8;
typedef cgc_int16_t cgc_s16;
typedef cgc_int32_t cgc_s32;
typedef cgc_int64_t cgc_s64;

typedef cgc_uint8_t cgc_u8;
typedef cgc_uint16_t cgc_u16;
typedef cgc_uint32_t cgc_u32;
typedef cgc_uint64_t cgc_u64;

typedef enum cgc_action_t cgc_action_t;
enum cgc_action_t {
  AUNUSED = 0,
  PLAY,
  PASS
};

typedef enum cgc_color_t cgc_color_t;
enum cgc_color_t {
  CUNUSED = 0,
  BLACK,
  WHITE,
  EMPTY,
  OFF_BOARD
};

#define error(__status) cgc_exit(__status)

#define BOARD_DIM 19
#define NUM_PLAYERS 2
#define NUM_HISTORIES 16
#define SET_BOARD(board, x, y, pt) (board[y][x] = pt)
#define COLOR_STRING(c) (c == WHITE ? "White" : "Black")
#define OTHER_PLAYER(c) (c == WHITE ? BLACK : WHITE)

typedef cgc_color_t cgc_board_t[BOARD_DIM][BOARD_DIM];
typedef cgc_u8 cgc_sboard_t[BOARD_DIM][BOARD_DIM];

typedef cgc_u64 cgc_hash_t;

static cgc_u32 pcnt = 0;
static cgc_u64 prng_s = 0;
cgc_u64 cgc_prand(void)
{
  pcnt++;
  prng_s = prng_s * 1103515245 + 12345;
  return (cgc_u32)(prng_s / (2 << 15));
}

typedef struct cgc_hashl_t cgc_hashl_t;
struct cgc_hashl_t {
  cgc_hash_t hash;
  cgc_hashl_t* next;
};

static inline void cgc_INIT_LIST(cgc_hashl_t** list, cgc_hash_t hash)
{
  *list = cgc_calloc(1, sizeof(cgc_hashl_t));
  if (!list) error(1);
  (*list)->hash = hash;
  (*list)->next = *list;
}

static inline void cgc_EXTD_LIST(cgc_hashl_t* list, cgc_hash_t hash)
{
  cgc_hashl_t* new = cgc_calloc(1, sizeof(cgc_hashl_t));
  if (!new)
    error(1);
  for (; list->next != list; list = list->next)
    if (list->hash == hash)
      return;

  new->next = new;
  new->hash = hash;
  list->next = new;
}

static inline int cgc_CONT_LIST(cgc_hashl_t* list, cgc_hash_t hash)
{
  cgc_u32 cnt = 1;
  do {
    if (list->hash == hash) {
      return 1;
    } else if (list->next != list) {
      list = list->next;
      cnt++;
    } else {
      break;
    }
  } while (1);

  return 0;
}

typedef struct cgc_game_t cgc_game_t;
struct cgc_game_t {
  cgc_board_t board;
  cgc_u32 scores[NUM_PLAYERS];
  cgc_u32 caps[NUM_PLAYERS];
  cgc_u32 used[NUM_PLAYERS];
  cgc_u64 ticks;
  cgc_u8 passes;
  cgc_color_t current;
  cgc_hashl_t* history;
};

static cgc_hash_t ztable[BOARD_DIM * BOARD_DIM][NUM_PLAYERS];
int cgc_init_zobrist(void)
{
  for (cgc_u16 y = 0; y < BOARD_DIM * BOARD_DIM; y++) {
    for (cgc_u16 x = 0; x < NUM_PLAYERS; x++) {
      ztable[y][x] = cgc_prand();
    }
  }
  return 0;
}

cgc_hash_t cgc_hash_zobrist(cgc_board_t board)
{
  cgc_hash_t h = 0;
  for (cgc_u16 y = 0; y < BOARD_DIM * BOARD_DIM; y++) {
    if (board[y / BOARD_DIM][y % BOARD_DIM] != EMPTY) {
      h ^= ztable[y][board[y / BOARD_DIM][y % BOARD_DIM] - 1];
    }
  }
  return h;
}

void cgc_push_game_state(cgc_game_t* game) {
  cgc_hash_t h = cgc_hash_zobrist(game->board);
  cgc_EXTD_LIST(game->history, h);
}

static inline cgc_u64 cgc_abs(cgc_s32 x)
{
  return (x < 0) ? -x : x;
}

cgc_u8 cgc_ndigits(int x)
{
  if (x == 0)
    return 1;

  cgc_u8 n = 0;
  while (x) {
    x /= 10;
    n++;
  }
  return n;
}

cgc_game_t* cgc_init_game(cgc_board_t board)
{
  cgc_game_t* game = cgc_calloc(1, sizeof(cgc_game_t));
  if (!game)
    return NULL;

  if (board) {
    for (cgc_u32 y = 0; y < BOARD_DIM; y++) {
      for (cgc_u32 x = 0; x < BOARD_DIM; x++) {
        game->board[y][x] = board[y][x];
      }
    }
  } else {
    for (cgc_u32 y = 0; y < BOARD_DIM; y++) {
      for (cgc_u32 x = 0; x < BOARD_DIM; x++) {
        game->board[y][x] = EMPTY;
      }
    }
  }

  for (cgc_u32 i = 0; i < NUM_PLAYERS; i++) {
    game->scores[i] = 0;
    game->caps[i] = 0;
    game->used[i] = 0;
  }

  game->history = NULL;
  cgc_hash_t h = cgc_hash_zobrist(game->board);
  cgc_INIT_LIST(&game->history, h);
  game->ticks = 0;
  game->passes = 0;

  return game;
}

cgc_game_t* cgc_copy_game(cgc_game_t* game)
{
  cgc_game_t* copy = cgc_calloc(1, sizeof(cgc_game_t));
  if (!copy)
    error(1);
  cgc_memcpy(copy, game, sizeof(cgc_game_t));
  return copy;
}

cgc_color_t cgc_get_color(cgc_board_t board, cgc_u8 x, cgc_u8 y)
{
  if (x >= BOARD_DIM || x < 0 || y >= BOARD_DIM || y < 0)
    return OFF_BOARD;

  return board[y][x];
}

int cgc_has_liberty(cgc_game_t* game, cgc_sboard_t sboard, cgc_u8 x, cgc_u8 y, cgc_color_t color)
{
  cgc_color_t c = cgc_get_color(game->board, x, y);
  if (c == OFF_BOARD)
    return 0;
  if (sboard[y][x] == 1)
    return 0;

  cgc_color_t u = cgc_get_color(game->board, x, y + 1);
  cgc_color_t d = cgc_get_color(game->board, x, y - 1);
  cgc_color_t l = cgc_get_color(game->board, x - 1, y);
  cgc_color_t r = cgc_get_color(game->board, x + 1, y);

  sboard[y][x] = 1;
  if (u == EMPTY || d == EMPTY || l == EMPTY || r == EMPTY) {
    return 1;
  }

  if (u == color && cgc_has_liberty(game, sboard, x, y + 1, color)) {
    return 1;
  } else if (d == color && cgc_has_liberty(game, sboard, x, y - 1, color)) {
    return 1;
  } else if (l == color && cgc_has_liberty(game, sboard, x - 1, y, color)) {
    return 1;
  } else if (r == color && cgc_has_liberty(game, sboard, x + 1, y, color)) {
    return 1;
  }

  return 0;
}

cgc_u8 cgc_surrounded_by(cgc_game_t* game, cgc_sboard_t sboard, cgc_u8 x, cgc_u8 y, cgc_color_t color)
{
  if (!game || !sboard)
    error(1);

  cgc_color_t cur = cgc_get_color(game->board, x, y);
  if (cur == OFF_BOARD || sboard[y][x] == 1)
    return 1;

  if (cur == color) {
      sboard[y][x] = 1;
      return 2;
  } else if (cur == OTHER_PLAYER(color)) {
      sboard[y][x] = 1;
      return 0;
  } else if (cur == EMPTY) {
      sboard[y][x] = 1;
  }

  cgc_u8 u = cgc_surrounded_by(game, sboard, x, y + 1, color);
  cgc_u8 d = cgc_surrounded_by(game, sboard, x, y - 1, color);
  cgc_u8 l = cgc_surrounded_by(game, sboard, x + 1, y, color);
  cgc_u8 r = cgc_surrounded_by(game, sboard, x - 1, y, color);

  if (u + d + l + r > 4) {
    return 2;
  } else {
    return 0;
  }
}


int cgc_remove_captures(cgc_game_t* game, cgc_color_t color)
{
  cgc_u32 cnt = 0;
  cgc_sboard_t sboard;
  cgc_game_t* frozen = cgc_copy_game(game);
  for (cgc_u8 y = 0; y < BOARD_DIM; y++) {
    for (cgc_u8 x = 0; x < BOARD_DIM; x++) {
      cgc_memset(sboard, 0, sizeof(sboard));
      if (cgc_get_color(frozen->board, x, y) == color && !cgc_has_liberty(frozen, sboard, x, y, color)) {
        cnt++;
        SET_BOARD(game->board, x, y, EMPTY);
      }
    }
  }
  cgc_free(frozen);
  return cnt;
}

int cgc_has_happened(cgc_game_t* proposed, cgc_game_t* current)
{
  cgc_hash_t h = cgc_hash_zobrist(proposed->board);
  if (cgc_CONT_LIST(current->history, h)) {
    return 1;
  }
  return 0;
}

int cgc_score(cgc_game_t* game, cgc_u32* black, cgc_u32* white)
{
  cgc_sboard_t sboard;
  *black = game->caps[BLACK - 1];
  *white = game->caps[WHITE - 1];

  for (cgc_u8 y = 0; y < BOARD_DIM; y++) {
    for (cgc_u8 x = 0; x < BOARD_DIM; x++) {
      cgc_memset(sboard, 0, sizeof(sboard));
      cgc_color_t cur = cgc_get_color(game->board, x, y);
      if (cur == WHITE) {
        *white += 1;
        continue;
      } else if (cur == BLACK) {
        *black += 1;
        continue;
      }

      cgc_u8 s = cgc_surrounded_by(game, sboard, x, y, BLACK);
      if (s > 1) {
        *black += 1;
        continue;
      }

      s = cgc_surrounded_by(game, sboard, x, y, WHITE);
      if (s > 1) {
        *white += 1;
        continue;
      }
    }
  }

  return 0;
}

int cgc_check_and_play(cgc_game_t** game, cgc_u8 x, cgc_u8 y, cgc_color_t color, cgc_u8 play)
{
  if (color != EMPTY && (*game)->board[y][x] != EMPTY) {
    return -1;
  }


  cgc_game_t* simulated = cgc_copy_game(*game);
  SET_BOARD(simulated->board, x, y, color);
  cgc_u16 our_cap = cgc_remove_captures(simulated, OTHER_PLAYER(color));
  cgc_u16 thr_cap = cgc_remove_captures(simulated, color);

  if (cgc_has_happened(simulated, *game)) {
    cgc_free(simulated);
    return -1;
  } else if (play) {
    cgc_free(*game);
    *game = simulated;
    (*game)->used[color - 1]++;
    (*game)->caps[color - 1] += our_cap;
    (*game)->caps[OTHER_PLAYER(color) - 1] += thr_cap;
    return 0;
  } else {
    cgc_free(simulated);
    return 0;
  }
}

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 30
#define BOARD_WIDTH 37

static inline void cgc_clear_screen(void)
{
  cgc_fdprintf(STDOUT, "\033[2J");
}

static inline void cgc_print_char(char color)
{
  cgc_fdprintf(STDOUT, "%c", color);
}

void cgc_print_rchar(char c, cgc_u8 r)
{
  for (cgc_u8 i = 0; i < r; i++)
    cgc_print_char(c);
}

void cgc_print_player(cgc_game_t* game, cgc_color_t color)
{
#define OFFSET 7
  cgc_fdprintf(STDOUT, "%s: %d", COLOR_STRING(color), game->scores[color - 1]);
  cgc_print_rchar(' ', (((SCREEN_WIDTH - BOARD_WIDTH) / 2) - OFFSET) - cgc_ndigits(game->scores[color - 1]));
}

void cgc_print_point(cgc_game_t* game, cgc_u8 x, cgc_u8 y)
{
  if (x != 0)
    cgc_print_char(' ');

  switch (cgc_get_color(game->board, x, y)) {
  case EMPTY:
    cgc_print_char('-');
    break;
  case WHITE:
    cgc_print_char('W');
    break;
  case BLACK:
    cgc_print_char('B');
    break;
  default:
    return;
  }

  if (x != BOARD_DIM - 1)
    cgc_print_char(' ');
  else
    cgc_print_char('\n');
}

void cgc_draw_game(cgc_game_t* game)
{
  //cgc_clear_screen();

  cgc_print_rchar('\n', (SCREEN_HEIGHT - BOARD_DIM) / 2);

  for (cgc_u32 y = 0; y < BOARD_DIM; y++) {
    if (cgc_abs(y - (BOARD_DIM / 2)) == 3) {
      cgc_print_player(game, y < BOARD_DIM / 2 ? WHITE : BLACK);
    } else if (y == BOARD_DIM / 2) {
      cgc_fdprintf(STDOUT, "Ticks: %u", game->ticks);
      cgc_print_rchar(' ', (((SCREEN_WIDTH - BOARD_WIDTH) / 2) - OFFSET)- cgc_ndigits(game->ticks));
    } else {
      cgc_print_rchar(' ', (SCREEN_WIDTH - BOARD_WIDTH) / 2);
    }
    for (cgc_u32 x = 0; x < BOARD_DIM; x++) {
      cgc_print_point(game, x, y);
    }
  }

  cgc_print_rchar('\n', (SCREEN_HEIGHT - BOARD_DIM) / 2);
}

void cgc_sleep(int s, int us)
{
  struct cgc_timeval t;
  t.tv_sec = s;
  t.tv_usec = us;
  cgc_fdwait(0, NULL, NULL, &t, NULL);
}

int cgc_read_n_bytes(int fd, size_t n, char *buf, int has_terminator, char terminator)
{
  if (!n || !buf)
    return -1;

  size_t rx = 0, total_read = 0;

  while (total_read < n) {
    if (receive(fd, buf + total_read, 1, &rx) != 0) {
      return -1;
    } else if (rx == 0) {
      break;
    } else {
      if (has_terminator && *(buf + total_read) == terminator) {
        *(buf + total_read) = '\0';
        break;
      }
      total_read += rx;
    }
  }

  return total_read;
}

int cgc_read_move(cgc_u8* x, cgc_u8* y, cgc_u8* pass)
{
#define INPUT_MAX 8
  int ret = -1;
  char buf[INPUT_MAX + 1];
  *pass = 0;
  cgc_memset(buf, 0, INPUT_MAX);
  cgc_read_n_bytes(STDIN, INPUT_MAX, buf, 1, '\n');
  buf[INPUT_MAX] = '\0';

  char* p = NULL;

  if (!cgc_strncmp("pass", buf, cgc_strlen("pass"))) {
    *pass = 1;
    ret = 0; goto out;
  }

  long n1 = cgc_strtol(buf, &p, 10);

  if (p == buf)
    goto out;

  if (p >= buf + INPUT_MAX)
    goto out;

  while (p && *p && p < buf + INPUT_MAX && !cgc_isdigit(*p))
    p++;

  long n2 = cgc_strtol(p, NULL, 10);

  if (n1 >= 0 && n1 < BOARD_DIM && n2 >= 0 && n2 < BOARD_DIM) {
    *x = n1;
    *y = n2;
    ret = 0; goto out;
  }
out:
  return ret;
}

void cgc_end_game(cgc_game_t* game, char *name, cgc_u8 reason)
{
  cgc_free(game);
  cgc_fdprintf(STDOUT, "Game Over ");
  switch (reason) {
  case 0:
    cgc_fdprintf(STDOUT, "Both Passed\n");
    break;
  case 1:
    cgc_fdprintf(STDOUT, "Game Time Exceeded\n");
    break;
  case 2:
    cgc_fdprintf(STDOUT, "Stones Exhausted\n");
    break;
  }

  cgc_fdprintf(STDOUT, "You are a ");
  cgc_fdprintf(STDOUT, ((game->scores[BLACK - 1] > game->scores[WHITE - 1])) ? "Winner, " : "Loser, ");
#ifdef PATCHED
  cgc_fdprintf(STDOUT, "%s", name);
#else
  cgc_fdprintf(STDOUT, name);
#endif

  cgc_exit(0);
  return;
}

void cgc_prompt_move(cgc_game_t* game, cgc_color_t color)
{
  cgc_fdprintf(STDOUT, "%d ", game->ticks);
  if (color == BLACK)
    cgc_fdprintf(STDOUT, "B >");
  else if (color == WHITE)
    cgc_fdprintf(STDOUT, "W >");
}

char* cgc_color_to_string(cgc_color_t c)
{
  switch (c) {
  case WHITE:
    return "White";
  case BLACK:
    return "Black";
  case EMPTY:
    return "Empty";
  case OFF_BOARD:
    return "Off";
  default:
    error(1);
  }

  return NULL;
}

int cgc_pass_for(cgc_game_t* game, cgc_color_t color)
{
  game->passes++;
  if (color == WHITE && game->passes > 1)
    return -1;
  return 0;
}

int cgc_interact(cgc_game_t** game, cgc_color_t color)
{
  cgc_u8 x, y, pass;

  cgc_prompt_move(*game, color);
  if (cgc_read_move(&x, &y, &pass) < 0) {
    return -1;
  }

  if (pass) {
    return cgc_pass_for(*game, color) < 0;
  }

  (*game)->passes = 0;
  if (cgc_check_and_play(game, x, y, color, 1) < 0) {
    return -1;
  }

  cgc_push_game_state(*game);
  return 0;
}

cgc_action_t cgc_calculate_move(cgc_game_t** game, cgc_u8* ox, cgc_u8* oy, cgc_color_t color)
{
  cgc_s32 vote, bvote = 0;
  cgc_u8 bx, by;
  cgc_sboard_t sboard;
  cgc_u32 chance;
  cgc_u8 gend = 0;

  for (cgc_u8 y = 0; y < BOARD_DIM; y++) {
    for (cgc_u8 x = 0; x < BOARD_DIM; x++) {
      vote = 0;
      cgc_memset(sboard, 0, sizeof(sboard));

      cgc_u8 neigh_cnt = 0;
      neigh_cnt += (cgc_get_color((*game)->board, x + 1, y) == color);
      neigh_cnt += (cgc_get_color((*game)->board, x - 1, y) == color);
      neigh_cnt += (cgc_get_color((*game)->board, x, y + 1) == color);
      neigh_cnt += (cgc_get_color((*game)->board, x, y - 1) == color);

      if (neigh_cnt == 4)
        vote -= 3;
      else if (neigh_cnt == 2)
        vote += 1;
      else if (neigh_cnt == 1)
        vote += 2;

      if (vote >= bvote) {
        if (cgc_check_and_play(game, x, y, color, 0) < 0) {
          continue;
        }

        chance = cgc_prand();
        if (chance % 100 > (vote == bvote ? 90 : 75)) {
          gend++;
          bx = x;
          by = y;
          bvote = vote;
          if (gend > 5)
            goto done;
        }
      }
    }
  }

done:
  if (!gend)
    return PASS;
  else {
    *ox = bx, *oy = by;
    return PLAY;
  }
}

int cgc_ai(cgc_game_t** game, cgc_color_t color)
{
  cgc_u8 x, y;
  switch (cgc_calculate_move(game, &x, &y, color)) {
    case PLAY:
      cgc_check_and_play(game, x, y, color, 1);
      cgc_push_game_state(*game);
      break;
    case PASS:
      (*game)->passes++;
      break;
    default:
      error(1);
  }

  return 0;
}

void cgc_pb(cgc_game_t* game)
{
  cgc_fdprintf(STDERR, "@@");
  for (cgc_u16 y = 0; y < BOARD_DIM; y++) {
    for (cgc_u16 x = 0; x < BOARD_DIM; x++) {
      cgc_fdprintf(STDERR, "%d", game->board[y][x]);
    }
  }
  cgc_fdprintf(STDERR, "\n");
}

int main(void)
{
#define NAME_LEN 8
  char name[NAME_LEN];
  int iret = -1;

  cgc_read_n_bytes(STDIN, sizeof(prng_s), (char *)&prng_s, 0, 0);
  if (cgc_init_zobrist() != 0)
    error(1);

  cgc_game_t* game = cgc_init_game(NULL);
  if (!game)
    error(1);

  cgc_fdprintf(STDOUT, "What is your name?\n");
  if (cgc_read_n_bytes(STDIN, NAME_LEN, name, 1, '\n') < 0) {
    printf("Need a name, bro\n");
    cgc_exit(1);
  }

  printf("Hi, %s\n", name);
  cgc_draw_game(game);
  while (1) {
    while (1) {
      int r = cgc_interact(&game, BLACK);
      if (r == -1) {
        continue;
      } else if (r == 0) {
        break;
      } else if (r == 1) {
        cgc_end_game(game, name, 0);
      }
    }

    cgc_score(game, &(game->scores[BLACK - 1]), &(game->scores[WHITE -1]));

    cgc_draw_game(game);
    cgc_ai(&game, WHITE);
    cgc_score(game, &(game->scores[BLACK - 1]), &(game->scores[WHITE - 1]));

#define MAX_STONES 90
#define MAX_TICKS 150
    game->ticks++;
    if (game->ticks > MAX_TICKS)
      cgc_end_game(game, name, 1);
    if (game->used[BLACK - 1] > MAX_STONES || game->used[WHITE - 1] > MAX_STONES)
      cgc_end_game(game, name, 2);
  }
}
