#ifndef __TOKENIZE_H_
#define __TOKENIZE_H_

#include "list.h"

DEF_LIST(tok, char *);

cgc_tok_list *cgc_tokenize(char *expr);

#endif /* __TOKENIZE_H_ */
