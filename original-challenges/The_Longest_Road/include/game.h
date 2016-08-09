/*

Author: James Nuttall (james@cromulence.co)

Copyright (c) 2015 Cromulence LLC

Permission is hereby granted, cgc_free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/
#ifndef __GAME_H__
#define __GAME_H__

#define MAX_PIECES 128

// each piece has a pointer for each of its sides
// if pointer is null, no road on that side
// if pointer is not null but doesn't point to another piede yet, it will be set to ROAD
// if pointer is not null and not equal to ROAD, it points to another piece, 
//   with the cgc_Road_Link's side set to the side of the piece we're connecting to
#define ROAD -3
#define NO_ROAD -4

// describes the piece and side where a road connects to
typedef struct 
{
	char target; // number of target piece this connects to
	char side;  // 0-3, which side on the target piece this road connects with
} cgc_Road_Link;


// describes a game piece
// each side can be null (no road)
// or can be a pointer to another game piece
typedef struct
{
	// describes each side and what it points to
	char piece_num; // just keeps track of each piece
	cgc_Road_Link sides[4];
} cgc_Piece;

typedef struct
{
	cgc_Piece stack[MAX_PIECES];
	int top_element; // how many pieces in this stack (-1 because zero counting)?
} cgc_Game_Stack;

cgc_Game_Stack game_stack;

// stores
extern char current_max_road_len;

extern int goal_len; // length the user is trying to reach

void cgc_push_piece(cgc_Piece *piece);
void cgc_pop_piece(cgc_Piece *piece);
int cgc_connect_pieces(cgc_Piece *pieceA, char sideA, cgc_Piece *pieceB, char sideB);
int cgc_get_max_road_len();
int cgc_isTopPiecePlaced();
void cgc_piece_to_pkt(cgc_Piece *piece, char* str);
int cgc_discard_piece();
int cgc_create_random_piece(cgc_Piece *piece);
int cgc_get_piece(char pce);

#endif