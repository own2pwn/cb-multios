#ifndef __FORM_H__
#define __FORM_H__

#define NUM_CMDS 5
#define MAX_CMD_SIZE 16

typedef struct question {
  const char *title;
  const char *hint;
  char *answer;
  char optional;
  struct question *prev, *next;
  int (*validator) (char *input);
} cgc_question_t;

typedef struct page {
  const char *title;
  struct page *prev, *next;
  cgc_question_t *questions;
  char completed;
} cgc_page_t;

typedef struct form {
  const char *greeting;
  const char *ending;
  const char *help;
  void (*init) (void);
  int (*handlers[NUM_CMDS]) (struct form *_form, char *arg);
  cgc_page_t *pages;
  cgc_page_t *cur_page;
  cgc_question_t *cur_question;
} cgc_form_t;

int cgc_handle_exit(cgc_form_t *form, char *arg);
int cgc_handle_help(cgc_form_t *form, char *arg);
int cgc_handle_next(cgc_form_t *form, char *arg);
int cgc_handle_prev(cgc_form_t *form, char *arg);
int cgc_handle_update(cgc_form_t *form, char *arg);
int cgc_handle_line(cgc_form_t *form, char *arg);
void cgc_print_prompt(cgc_form_t *form, int with_page, int done);
void cgc_print_greeting(cgc_form_t *form);

#define DEC_Q(_name, _title, _hint, _optional, _validator)  \
  static cgc_question_t _name = {                               \
    .title = _title,                                        \
    .hint = _hint,                                          \
    .answer = NULL,                                         \
    .optional = _optional,                                  \
    .prev = NULL,                                           \
    .next = NULL,                                           \
    .validator = _validator                                 \
  }

#define DEC_P(_name, _title)  \
  static cgc_page_t _name = {     \
    .title = _title,          \
    .prev = NULL,             \
    .next = NULL,             \
    .questions = NULL,        \
    .completed = 0            \
}

#define LINK(e, _prev, _next) \
  do {                        \
    e.prev = _prev;           \
    e.next = _next;           \
  } while (0)

#endif /* __FORM_H__ */
