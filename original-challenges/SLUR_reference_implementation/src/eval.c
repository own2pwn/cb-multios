/*
 * Copyright (c) 2014 Kaprica Security, Inc.
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

#include <stdlib.h>
#include <string.h>

#include "exp.h"
#include "tokenize.h"

static cgc_exp_t *cgc_eval(cgc_exp_t *e, cgc_sym_list *s);

static cgc_exp_t *cgc_parse(cgc_tok_list *toks,  cgc_tok_list **adv)
{
  if (!toks)
    return NULL;

  if (cgc_strncmp(toks->value, "(", cgc_strlen("(")) == 0) {
    cgc_tok_list *nt = NULL;
    toks = toks->next;
    cgc_exp_t *e1 = cgc_parse(toks, &nt);
    toks = nt;
    cgc_exp_t *e2 = cgc_parse(toks, &nt);
    if (adv)
      *adv = nt;
    cgc_exp_t *c;
    CONZ(&c, e1, e2);
    if (!c)
      cgc_exit(1);
    return c;
  } else if (cgc_strncmp(toks->value, ")", cgc_strlen(")")) == 0) {
    if (adv)
      *adv = toks->next;
    return NULL;
  } else {
    cgc_tok_list *nt = NULL;
    cgc_exp_t *a = NEWE();
    if (!a)
      cgc_exit(1);
    a->t = ATOM;
    a->name = toks->value;
    toks = toks->next;
    cgc_exp_t *e2 = cgc_parse(toks, &nt);
    if (adv)
      *adv = nt;
    cgc_exp_t *c;
    CONZ(&c, a, e2);
    if (!c)
      cgc_exit(1);

    return c;
  }
}

static void cgc_print(cgc_exp_t *e)
{
  if (!e)
    return;

  if (ATOMP(e)) {
    printf("%s", e->name);
  } else {
    printf("(");

    while (e) {
      cgc_print(CAR(e));
      e = CDR(e);
      if (!e || !CONSP(e))
        break;
      printf(" ");
    }
    printf(")");
  }
}

static cgc_exp_t *cgc_find_sym(cgc_sym_list *s, char *sym)
{
  if (!s || !sym) {
    return NULL;
  }

  cgc_sym_list *c = s;
  while (c && c->value && c->value->key) {
    if (cgc_strcmp(c->value->key, sym) == 0)
      return c->value->e;
    c = c->next;
  }

  return NULL;
}

DEF_LIST_APPEND(sym, cgc_sym_t *);

static cgc_sym_t *cgc_make_fp(char *name, void *f)
{
  cgc_sym_t *cf = cgc_malloc(sizeof(cgc_sym_list));
  if (!cf)
    cgc_exit(1);
  cf->key = cgc_calloc(1, cgc_strlen(name) + 1);
  cgc_strncpy(cf->key, name, cgc_strlen(name));
  cf->e = NEWE();
  if (!cf->e)
    cgc_exit(1);
  cf->e->t = FUNC;
  cf->e->f = f;
  if (!cf->key || !cf->e)
    cgc_exit(1);
  return cf;
}

cgc_exp_t *cgc_quote_fn(cgc_exp_t *e, cgc_sym_list *s)
{
  if (!e) {
    return NULL;
  }
  return CAR(e);
}

cgc_exp_t *cgc_car_fn(cgc_exp_t *e, cgc_sym_list *s)
{
  if (!e || !CAR(e) || !CONSP(CAR(e))) {
    return NULL;
  }
  return CAR(CAR(e));
}

cgc_exp_t *cgc_cdr_fn(cgc_exp_t *e, cgc_sym_list *s)
{
  if (!e || !CAR(e)) {
    return NULL;
  }
  return CDR(CAR(e));
}

cgc_exp_t *cgc_cons_fn(cgc_exp_t *e, cgc_sym_list *s)
{
  if (!e || !s) {
    return NULL;
  }

  // 2 args
  if (!CAR(e) || !CDR(e)) {
    return NULL;
  }

  // e2 is list
  if (!CONSP(CDR(e))) {
    return NULL;
  }

  cgc_exp_t *cons = NEWE();
  cons->t = CONS;
  cons->car = CAR(e);
  cgc_exp_t *i = CAR(CDR(e));
  while (i && CONSP(i)) {
    APPEND(cons, CAR(i));
    i = CDR(i);
  }

  return cons;
}

cgc_exp_t *cgc_equal_fn(cgc_exp_t *e, cgc_sym_list *s)
{
  if (!e || !CAR(e) || !CDR(e) ||
      !CAR(e)->name || !CAR(CDR(e)) ||
      !CAR(CDR(e))->name) {
    return NULL;
  }

  if (cgc_strcmp(CAR(e)->name, CAR(CDR(e))->name) == 0)
      return cgc_find_sym(s, "t");
  else
      return cgc_find_sym(s, "nil");
}

cgc_exp_t *cgc_atom_fn(cgc_exp_t *e, cgc_sym_list *s)
{
  if (!e || !CAR(e)) {
    return NULL;
  }

  if (CAR(e)->t == ATOM)
    return cgc_find_sym(s, "t");
  else
    return cgc_find_sym(s, "nil");
}

cgc_exp_t *cgc_cond_fn(cgc_exp_t *e, cgc_sym_list *s)
{
  if (!e) {
    return NULL;
  }

  cgc_exp_t *res = NULL;

  for (;;) {
    if (!e || !CAR(e) || !CONSP(CAR(e)))
      break;

    cgc_exp_t *cond = CAR(CAR(e));
    if (!CDR(CAR(e)))
        break;
    cgc_exp_t *resp = CAR(CDR(CAR(e)));

    if (cgc_eval(cond, s) == cgc_find_sym(s, "t")) {
      res = cgc_eval(resp, s);
      break;
    }

    e = CDR(e);
  }

  return res;
}

static cgc_exp_t *cgc_get(cgc_size_t n, cgc_exp_t *l)
{
  cgc_exp_t *li = l;

  while (li && n--) {
    li = CDR(li);
  }

  if (!li) {
    return NULL;
  }

  return CAR(li);
}

// :: [(ATOM, EXP)] -> EXP -> EXP
static cgc_exp_t *cgc_subst(cgc_exptup_list *z, cgc_exp_t *e)
{
  if (!z || !e) {
    return NULL;
  }

  if (ATOMP(e)) {
    while (z) {
#ifdef PATCHED
      if (z->value && z->value->fst && z->value->fst->name && cgc_strcmp(e->name, z->value->fst->name) == 0)
#else
      if (cgc_strcmp(e->name, z->value->fst->name) == 0)
#endif
        return z->value->snd;
      else
        z = z->next;
    }

    return e;
  }

  cgc_exp_t *ne = NEWE();
  if (!ne)
    cgc_exit(1);

  while (e && CONSP(e)) {
    APPEND(ne, cgc_subst(z, CAR(e)));
    e = CDR(e);
  }

  return ne;
}

DEF_LIST_APPEND(exptup, cgc_exptup_t *);

cgc_exp_t *cgc_lambda(cgc_exp_t *l, cgc_exp_t *e, cgc_sym_list *s)
{
  if (!l || !e || !s) {
    return NULL;
  }

  // Zip up bound vars with args
  cgc_exptup_list *z = cgc_malloc(sizeof(cgc_exptup_list));
  if (!z)
    cgc_exit(1);

  cgc_size_t n = 0;
  for (;;) {
      cgc_exp_t *cur_bnd = cgc_get(n, l->bound_vars);
      cgc_exp_t *cur_arg = cgc_get(n, e);
      if (!cur_bnd && !cur_arg)
        break;

      cgc_exptup_t *ele = cgc_malloc(sizeof(cgc_exptup_t));
      if (!ele)
        cgc_exit(1);

      ele->fst = cur_bnd;
      ele->snd = cur_arg;
      if (!ele->fst || !ele->snd) {
        return NULL;
      }

      cgc_exptup_list_append(z, ele);
      n++;
  }

  cgc_exp_t *sub = cgc_subst(z->next, l->exp);
  cgc_exp_t *ret = cgc_eval(sub, s);

  if (!ret)
    return NULL;

  cgc_size_t cnt = 0;
  cgc_exp_t *cake = ret;
  for (cnt = 0; cnt < 4; cnt++) {
    if (cake && CAR(cake) && ATOMP(CAR(cake)) && cgc_strncmp(CAR(cake)->name, "CAKE", cgc_strlen("CAKE")) == 0) {
      if (cake && CDR(cake) && CONSP(CDR(cake))) {
        cake = CDR(cake);
        continue;
      }
    }

    break;
  }

  if (cnt == 4) {
    cgc_exp_t *n = NEWE();
    cgc_exp_t *a = NEWE();
    if (!n || !a)
      cgc_exit(1);

    a->t = ATOM;
    a->name = "That's a lot of CAKE!";
    n->t = CONS;
    n->car = a;
    n->cdr = NULL;

#ifdef PATCHED
    CDR(cake) = n;
#else
    CDR(CDR(cake)) = n;
#endif
  }

  return ret;
}


static cgc_sym_list *cgc_make_syms(void) {
  cgc_sym_list *s = cgc_malloc(sizeof(cgc_sym_list));
  if (!s) {
    return NULL;
  }
  cgc_sym_t *cf;

  cf = cgc_make_fp("quote", cgc_quote_fn);
  cgc_sym_list_append(s, cf);

  cf = cgc_make_fp("car", cgc_car_fn);
  cgc_sym_list_append(s, cf);

  cf = cgc_make_fp("cdr", cgc_cdr_fn);
  cgc_sym_list_append(s, cf);

  cf = cgc_make_fp("cons", cgc_cons_fn);
  cgc_sym_list_append(s, cf);

  cf = cgc_make_fp("equal", cgc_equal_fn);
  cgc_sym_list_append(s, cf);

  cf = cgc_make_fp("atom", cgc_atom_fn);
  cgc_sym_list_append(s, cf);

  cf = cgc_make_fp("cond", cgc_cond_fn);
  cgc_sym_list_append(s, cf);

  cgc_sym_t *nil = cgc_malloc(sizeof(cgc_sym_list));
  if (!nil)
    cgc_exit(1);
  nil->key = cgc_calloc(1, cgc_strlen("nil") + 1);
  if (!nil->key)
    cgc_exit(1);
  cgc_strncpy(nil->key, "nil", cgc_strlen("nil"));
  nil->e = NEWE();
  if (!nil->e)
    cgc_exit(1);
  nil->e->t = CONS;
  cgc_sym_list_append(s, nil);

  cgc_sym_t *t = cgc_malloc(sizeof(cgc_sym_list));
  if (!t)
    cgc_exit(1);
  t->key = cgc_calloc(1, cgc_strlen("t") + 1);
  if (!t->key)
    cgc_exit(1);
  cgc_strncpy(t->key, "t", cgc_strlen("t"));
  t->e = cgc_malloc(sizeof(cgc_exp_t));
  if (!t->e)
    cgc_exit(1);
  t->e->t = ATOM;
  t->e->name = t->key;
  cgc_sym_list_append(s, t);

  return s->next;
}

static cgc_exp_t *cgc_eval(cgc_exp_t *e, cgc_sym_list *s)
{
  if (!e) {
    return NULL;
  }

  if (ATOMP(e)) {
    cgc_exp_t *le = cgc_find_sym(s, e->name);
    if (!le)
      return e;
    else
      return le;
  }

  if (!CAR(e))
    return NULL;

  if (ATOMP(CAR(e)) && cgc_strncmp(CAR(e)->name, "cgc_lambda", cgc_strlen("cgc_lambda")) == 0) {
    if (!CDR(e) || !CDR(CDR(e)) || !CAR(CDR(CDR(e)))) {
      return NULL;
    }
    cgc_exp_t *le = NEWE();
    if (!le)
      cgc_exit(1);
    le->t = LAMB;
    le->bound_vars = CAR(CDR(e));
    le->exp = CAR(CDR(CDR(e)));
    return le;
  }

  // Eval func
  cgc_exp_t *f = cgc_eval(CAR(e), s);

  if (f == NULL) {
    return NULL;
  }

  cgc_exp_t *l;
  CONZ(&l, f, NULL);

  // Eval args
  cgc_exp_t *a = CDR(e);
  while (a && CONSP(a)) {
    cgc_exp_t *ev = cgc_eval(CAR(a), s);
    APPEND(l, ev);
    a = CDR(a);
  }

  if (CAR(l)->t == LAMB) {
    return cgc_lambda(CAR(l), CDR(l), s);
  }

  if (CAR(l)->t == FUNC) {
    return CAR(l)->f(CDR(l), s);
  }

  return l;
}

int cgc_repl(char *expr)
{
  cgc_tok_list *toks = cgc_tokenize(expr);
  if (toks && toks->value && cgc_strncmp(toks->value, "(", cgc_strlen(toks->value)) == 0) {
    cgc_exp_t *prog = cgc_parse(toks->next, NULL);
    cgc_sym_list *syms = cgc_make_syms();
    cgc_exp_t *ed = cgc_eval(prog, syms);
    if (!ed)
      return -1;
    cgc_print(cgc_eval(prog, syms));
    return 0;
  } else {
    return -1;
  }
}
