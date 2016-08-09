#ifndef HAND_H_
#define HAND_H_

#include "card.h"

typedef struct hand cgc_hand_t;
struct hand
{
    cgc_card_t *card;
    cgc_hand_t *next;
};

int cgc_get_card(cgc_hand_t **h, cgc_card_t *card);
int cgc_get_card_in_order(cgc_hand_t **h, cgc_card_t *card);
int cgc_replace_card(cgc_hand_t **h, cgc_card_t *new_card, cgc_card_t *old_card);
int cgc_remove_card(cgc_hand_t **h, cgc_card_t *card);
void cgc_discard_hand(cgc_hand_t **h);

#endif /* HAND_H_ */
