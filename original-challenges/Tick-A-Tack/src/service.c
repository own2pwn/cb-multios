/*
 * Copyright (C) Narf Industries <info@narfindustries.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
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
#include <libcgc.h>
#include "libc.h"
#include "tt.h"
#include "service.h"

cgc_tt_game * game;


void cgc_do_menu(size_t replay) {
    if(replay) {
        cgc_send(REPLAYCHOICES, cgc_strlen(REPLAYCHOICES));
    } else {
        cgc_send(INITCHOICES, cgc_strlen(INITCHOICES));
    }
}

void cgc_do_quit() {
    cgc_send(BYEBYE, cgc_strlen(BYEBYE));
}

void cgc_do_select_char() {
    char choice[3] = { 0 };
    char player_char = '\0';

    while(!player_char) {
        cgc_send(CHARCHOICE, cgc_strlen(CHARCHOICE));
        cgc_prompt_user(ROOTPROMPT, choice, 3, BADSTRERROR);
        // cgc_syslog(LOG_DEBUG, "cgc_do_select_char: choice: ~o\n", choice[0]);

        if(choice[0] == 'P') {
            player_char = 'P';
        } else if (choice[0] == 'Q') {
            player_char = 'Q';
        } else {
            cgc_send(WAT, cgc_strlen(WAT));            
        }
    }
    cgc_set_player_chars(game->data, player_char);
    if(LOGLEVEL == LOG_DEBUG) {cgc_syslog(LOG_DEBUG, "player: ~o\n", player_char);}

}

void cgc_send_current_board() {
    // board must be a char buff[(17 * 8) + 1] = char buf[137]
    char board[137] = { 0 };
    cgc_make_board(game->data, board);

    cgc_send(board, cgc_strlen(board));
    if(LOGLEVEL == LOG_DEBUG) {cgc_syslog(LOG_DEBUG, " board\n~c\n", board);}

}

// process a single move
void cgc_make_move() {
    int update_failed;
    cgc_tt_move_coords move = { 0 };
    char turn_coords[5] = { 0 }; // '<row> <col>\x07\0'
    // if player's turn, request coordinates
    if(cgc_is_players_turn(game->data)) {
        cgc_send(MAKEMOVE, cgc_strlen(MAKEMOVE));
        cgc_prompt_user(ROOTPROMPT, turn_coords, 5, BADSTRERROR);
        move.row = cgc_str2int(&turn_coords[0]);
        move.col = cgc_str2int(&turn_coords[2]);
        if(LOGLEVEL == LOG_DEBUG) {cgc_syslog(LOG_DEBUG, "move_move(): player\n");}
    } else { 
        // else, select random, but valid coordinates for computer's move
        cgc_computer_move(game->data, &move);
        if(LOGLEVEL == LOG_DEBUG) {cgc_syslog(LOG_DEBUG, "move_move(): computer\n");}
        // inform player where computer moved
        // build move message
        // 23 letters/\n/\0 + 2 for 2 single digit numbers
        char move_msg[25] = { 0 };
        cgc_snprintf(move_msg, 25, COMPMOVE, 
                    move.row, 
                    move.col); 

        // cgc_send computer move msg
        cgc_send(move_msg, cgc_strlen(move_msg));
        if(LOGLEVEL == LOG_DEBUG) {cgc_syslog(LOG_DEBUG, "move_move(): cgc_send computer's move ~n ~n.\n", move.row, move.col);}

    }

    if(LOGLEVEL == LOG_DEBUG) {cgc_syslog(LOG_DEBUG, "move_move(): coords '~n', '~n'\n", move.row, move.col);}
    // update board
    update_failed = cgc_update_board(game->data, &move);

    // if coordinates are bad, then board update failed
    if(update_failed) {
        // fuss at user
        if(cgc_is_players_turn(game->data)) {
            cgc_send(BADMOVE, cgc_strlen(BADMOVE));
        }
        if(LOGLEVEL == LOG_DEBUG) {cgc_syslog(LOG_DEBUG, "move_move(): bad move\n");}

    } else {
        cgc_move_complete(game->data);
        if(LOGLEVEL == LOG_DEBUG) {cgc_syslog(LOG_DEBUG, "move_move(): good move\n");}
    }
}

// inform player if they won or lost
// as well as the current win/loss total
void cgc_send_game_results() {

    if(cgc_is_player_winner(game->data)) {
        // cgc_send congrats
        cgc_send(WIN, cgc_strlen(WIN));
        if(LOGLEVEL == LOG_DEBUG) {cgc_syslog(LOG_DEBUG, "cgc_send_game_results(): player wins\n");}
    } else {
        // cgc_send loss msg
        cgc_send(LOSE, cgc_strlen(LOSE));
        if(LOGLEVEL == LOG_DEBUG) {cgc_syslog(LOG_DEBUG, "cgc_send_game_results(): computer wins\n");}
    }

    // build score message
    // 15 letters/\n/\0 + 20 for 2 uint32 numbers
    char score_msg[35] = { 0 };
    cgc_snprintf(score_msg, 35, SCORE, 
                game->scores->player, 
                game->scores->computer); 

    // cgc_send final scores
    cgc_send(score_msg, cgc_strlen(score_msg));
    if(LOGLEVEL == LOG_DEBUG) {cgc_syslog(LOG_DEBUG, "~c", score_msg);}
}

// play one game and report final results
void cgc_do_play() {

    // select char
    cgc_do_select_char();

    // play game
    cgc_send(GO, cgc_strlen(GO));

    while(1) {

        // cgc_send board
        // cgc_send_current_board();

        // process a move
        cgc_make_move();

        // end game and update scores when
        //  no more moves or if have winner
        if(cgc_have_winner(game->data) || !cgc_have_moves_remaining(game->data)) {
            cgc_update_score(game);
            break;
        }
    }

    // cgc_send final board
    // cgc_send_current_board();

    cgc_send_game_results();
}

int main(void) {
    char choice[14];
    int choice_buf_size = 14;
    size_t replay = 0;
    void * memory_base = NULL;
    void * memory_idx = NULL;

    // cgc_send INITMSG
    cgc_send(INITMSG, cgc_strlen(INITMSG));

    while (1) {

        // init game state memory
        // allocate the space for game state and setup pointers
        if(memory_base && !game->scores->player && !game->scores->computer) {
#ifdef PATCHED
            game = (cgc_tt_game *)memory_base;
#else
            game = (cgc_tt_game *)memory_idx;
#endif

            if(LOGLEVEL == LOG_DEBUG) {cgc_syslog(LOG_DEBUG, "Set game state.\n");}
        } else if(!memory_base) {
            // alloc space and setup game state pointers
            int memory_size = sizeof(cgc_tt_game) + sizeof(cgc_tt_data) + sizeof(cgc_tt_scores);
            int result = 0;
            result = allocate(memory_size, 0, &memory_base);

            if(result)
                _terminate(99);

            memory_idx = memory_base;
            game = (cgc_tt_game *)memory_idx;
            memory_idx += sizeof(cgc_tt_game);
            game->data = (cgc_tt_data *)memory_idx;
            memory_idx += sizeof(cgc_tt_data);
            game->scores = (cgc_tt_scores *)memory_idx;
            if(LOGLEVEL == LOG_DEBUG) {cgc_syslog(LOG_DEBUG, "Allocate and set game state.\n");}
        }

        // initialize game data
        cgc_init_game_data(game->data);

        // cgc_send MENU
        cgc_do_menu(replay);
        // cgc_send ROOTPROMPT
        cgc_prompt_user(ROOTPROMPT, choice, choice_buf_size, BADSTRERROR);

        // receive user input and check for COMMAND
        if (cgc_streq(choice, "PLAY") == 0) {
            cgc_do_play(); // replay should be 0 here
            replay = 1;
        } else if (replay && cgc_streq(choice, "PLAY AGAIN") == 0) {
            cgc_do_play(); // replay should be 1 here
        } else if (replay && cgc_streq(choice, "START OVER") == 0) {
            replay = 0;
            cgc_reset_scores(game->scores);
        } else if (cgc_streq(choice, "QUIT") == 0) {
            cgc_do_quit();
            break;
        } else {
            cgc_send(WAT, cgc_strlen(WAT));
            break;
        }
    }

    return 0;
}


