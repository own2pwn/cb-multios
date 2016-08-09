#ifndef DECK_H_
#define DECK_H_
#include "card.h"

#define CARDS_PER_SUIT 13
#define NUM_SUITS 4
#define NUM_CARDS (CARDS_PER_SUIT * NUM_SUITS)

#define RESHUFFLE_CARD (int)(.65 * NUM_CARDS)

typedef struct deck
{
    cgc_card_t cards[NUM_CARDS];
    int top_of_deck;
} cgc_deck_t;

void cgc_init_deck(cgc_deck_t *d, unsigned int *seed);
int cgc_shuffle_deck(cgc_deck_t *d);
int cgc_shuffle_deck_if_needed(cgc_deck_t *d);
cgc_card_t *cgc_draw_card(cgc_deck_t *d);

#endif /* DECK_H_ */
