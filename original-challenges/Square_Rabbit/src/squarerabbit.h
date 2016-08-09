#ifndef VSQUARERABBIT_H
#define VSQUARERABBIT_H

#include "card.h"
#include "hand.h"
#include "deck.h"

typedef struct squarerabbit cgc_squarerabbit_t;
struct squarerabbit {
    cgc_deck_t *deck;
    cgc_hand_t *dealer_hand;
    cgc_hand_t *player_hand;
    int current_bet;
    int paid_insurance;
    char player_finished;
    char double_or_split;
    char busted;

    char cgc_split_len;
};

void cgc_init_squarerabbit(unsigned int *seed);
cgc_squarerabbit_t *cgc_get_srabbit_game();
cgc_squarerabbit_t *cgc_get_split_hand();
int cgc_max_split();
int cgc_split_len();
int cgc_all_hands_busted();
int cgc_is_player_finished();
int cgc_calc_score(cgc_hand_t *srabbit_hand, int *is_soft);
int cgc_srabbit_payout();
int cgc_deal_srabbit(int bet);
void cgc_pay_insurance();
int cgc_check_player_squarerabbit(cgc_squarerabbit_t *srabbit);
int cgc_check_dealer_squarerabbit();
int cgc_check_dealer_squarerabbit2();
int cgc_hit();
int cgc_stand();
int cgc_double_down();
int cgc_can_split(cgc_squarerabbit_t *srabbit);
int cgc_split();
int cgc_dealer_hit();

#endif /* VSQUARERABBIT_H */
