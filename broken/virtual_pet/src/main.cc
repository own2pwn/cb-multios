
/*
 * Copyright (c) 2014 Jason L. Wright (jason@thought.net)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * virtual pet service... complete with heap buffer over flow into vtables!
 * yea!
 */

extern "C" {
#include <libcgc.h>
#include "cgc_libc.h"
#include "cgc_malloc.h"
};

#undef NULL
#define NULL (0L)

void cgc_put(char);
void cgc_put(const char *s);
void cgc_put(void *v);
void cgc_put(int v);
void cgc_put(long v);

void cgc_call_inits(void);
void cgc_terminate(int e) {_terminate(e);}

int first = 1;
int seq;

class cgc_Pet {
protected:
	char name[200];

public:
	cgc_Pet() { name[0] = '\0'; cgc_transmit_all(1, "init..\n", 7); }
	virtual ~cgc_Pet() = 0;

	virtual void cgc_Rollover() = 0;
	virtual void cgc_Speak() = 0;
	virtual void cgc_Flush() = 0;

	void cgc_Debug();
	void cgc_Name(const char *s) {
#ifdef PATCHED
		if (cgc_strlen(s) < sizeof(name))
			cgc_strlcat(name, s, sizeof(name));
#else
		if (cgc_strlen(s) < sizeof(name)) {
			char *n = name + cgc_strlen(name);
			cgc_memcpy(n, s, cgc_strlen(s) + 1);
		}
#endif
	}
};

template <typename T>
class cgc_listnode {
private:
	cgc_listnode<T> *nxt;
	T datax;

public:
	cgc_listnode<T> *cgc_next() { return nxt; }
	void cgc_next(cgc_listnode<T> *p) { nxt = p; }
	cgc_listnode(T d, cgc_listnode<T> *p) { datax = d; nxt = p; }
	cgc_listnode(T d) { datax = d; nxt = NULL; }
	T cgc_data() { return datax; }
};

class cgc_petlist {
	cgc_listnode<cgc_Pet *> *head;
	int n;

public:
	cgc_petlist() { head = NULL; n = 0; cgc_put("list init...\n"); }
	~cgc_petlist();

	void cgc_append(cgc_Pet *);
	void cgc_remove(int n);
	cgc_Pet *cgc_nth(int n);
	int cgc_len() { return n; }
} myList;

cgc_Pet::~cgc_Pet() {
}

void
cgc_Pet::cgc_Debug() {
	cgc_put((void *)this);
	cgc_put(' ');
	cgc_put((int)sizeof(cgc_Pet));
	cgc_put("\n");
}

class cgc_Cat: public cgc_Pet {
public:
	~cgc_Cat() {};
	virtual void cgc_Rollover();
	virtual void cgc_Speak();
	virtual void cgc_Flush();
};

class cgc_Dog: public cgc_Pet {
public:
	~cgc_Dog() {}
	virtual void cgc_Rollover();
	virtual void cgc_Speak();
	virtual void cgc_Flush();
};

class cgc_Hotdog: public cgc_Dog {
public:
	~cgc_Hotdog() { };
	virtual void cgc_Rollover();
	virtual void cgc_Speak();
	virtual void cgc_Flush();
};

void
cgc_Cat::cgc_Rollover() {
	cgc_put(name);
	cgc_put(" will not roll over\n");
	
}

void
cgc_Cat::cgc_Speak() {
	cgc_put(name);
	cgc_put(" meows\n");
}

void
cgc_Cat::cgc_Flush() {
	cgc_put(name);
	cgc_put(" purrs takes a nap\n");
}

void
cgc_Dog::cgc_Rollover() {
	cgc_put(name);
	cgc_put(" rolls over\n");
	
}

void
cgc_Dog::cgc_Speak() {
	cgc_put(name);
	cgc_put(" barks\n");
}

void
cgc_Dog::cgc_Flush() {
	cgc_put(name);
	cgc_put(" goes to sleep... permanently\n");
}

void
cgc_Hotdog::cgc_Rollover() {
	cgc_put(name);
	cgc_put(" rolls over on the grill\n");
	
}

void
cgc_Hotdog::cgc_Speak() {
	cgc_put(name);
	cgc_put(" can't speak\n");
}

void
cgc_Hotdog::cgc_Flush() {
	cgc_put(name);
	cgc_put(" tastes great with ketchup and mustard\n");
}

cgc_petlist::~cgc_petlist() {
	while (head) {
		cgc_listnode<cgc_Pet *> *p = head;
		head = head->cgc_next();
		delete p->cgc_data();
		delete p;
	}
}

void
cgc_petlist::cgc_append(cgc_Pet *pet) {
	if (n == 100) {
		cgc_put("too many pets already... cgc_put one to sleep\n");
		return;
	}

	n++;

	cgc_listnode<cgc_Pet *> *p = new cgc_listnode<cgc_Pet *>(pet);
	if (head == NULL)
		head = p;
	else {
		cgc_listnode<cgc_Pet *> *q;

		for (q = head; q->cgc_next(); q = q->cgc_next())
			/* empty */;
		q->cgc_next(p);
	}
}

cgc_Pet *
cgc_petlist::cgc_nth(int x) {
	cgc_listnode<cgc_Pet *>*p = head;

	if (n == 0) {
		cgc_put("you don't have any pets\n");
		return (NULL);
	}
	if (x < 0) {
		cgc_put("no imaginary pets allowed\n");
		return (NULL);
	}

	for (int i = 0; i < x; p = p->cgc_next(), i++)
		/* empty */;
	return p->cgc_data();
}

void
cgc_petlist::cgc_remove(int cgc_nth) {
	cgc_listnode<cgc_Pet *> *prev = NULL, *p;
	int i;

	if (n == 0) {
		cgc_put("you don't have any pets\n");
		return;
	}

	if (n < 0) {
		cgc_put("no imaginary pets allowed\n");
		return;
	}

	if (cgc_nth >= n) {
		cgc_put("you don't have that many pets\n");
		return;
	}

	for (i = 0, p = head; i < cgc_nth; i++, p = p->cgc_next())
		prev = p;

	n--;

	if (prev == NULL) {
		p = head;
		head = head->cgc_next();
		p->cgc_data()->cgc_Flush();
		delete p->cgc_data();
		delete p;
		return;
	}

	p->cgc_data()->cgc_Flush();
	delete p->cgc_data();
	prev->cgc_next(p->cgc_next());
	delete p;
}

void
cgc_do_rollover(const char *cmd, const char *opt) {
	for (int i = 0; i < myList.cgc_len(); i++)
		myList.cgc_nth(i)->cgc_Rollover();
}

void
cgc_do_speak(const char *cmd, const char *opt) {
	for (int i = 0; i < myList.cgc_len(); i++)
		myList.cgc_nth(i)->cgc_Speak();
}

void
cgc_do_create(const char *cmd, const char *opt) {
	cgc_Pet *p;

	if (!opt) {
		cgc_put("missing option\n");
		return;
	}

	if (!cgc_strcmp(opt, "hotdog"))
		p = new cgc_Hotdog();
	else if (!cgc_strcmp(opt, "cat"))
		p = new cgc_Cat();
	else if (!cgc_strcmp(opt, "dog"))
		p = new cgc_Dog();
	else {
		cgc_put("unknown pet\n");
		return;
	}

	myList.cgc_append(p);
}

void
cgc_do_name(const char *cmd, const char *opt) {
	int n = 0, i;
	const char *name;

	if (!opt) {
		cgc_printf("missing option\n");
		return;
	}

	if (opt[0] < '0' || opt[0] > '9') {
bad_num:
		cgc_put("bad number\n");
		return;
	}

	for (i = 0; opt[i]; i++) {
		if (opt[i] >= '0' && opt[i] <= '9')
			n = (n * 10) + opt[i] - '0';
		else if (opt[i] == ' ')
			break;
		else
			goto bad_num;
	}

	name = &opt[i + 1];
	if (opt[i] == '\0' || cgc_strlen(name) == 0) {
		cgc_put("missing name\n");
		return;
	}

	if (n >= myList.cgc_len()) {
		cgc_put("you don't have that many pets\n");
		return;
	}

	cgc_Pet *p = myList.cgc_nth(n);
	if (p == NULL)
		return;
	p->cgc_Name(name);
}

void
cgc_do_debug(const char *cmd, const char *opt) {
	for (int i = 0; i < myList.cgc_len(); i++)
		myList.cgc_nth(i)->cgc_Debug();
}

void
cgc_do_delete(const char *cmd, const char *opt) {
	int n = 0;

	if (!opt) {
		cgc_printf("missing option\n");
		return;
	}

	if (opt[0] < '0' || opt[0] > '9')
		goto bad_number;
	for (int i = 0; opt[i]; i++) {
		if (opt[i] >= '0' && opt[i] <= '9')
			n = n * 10 + opt[i] - '0';
		else
			goto bad_number;
	}

	myList.cgc_remove(n);

	return;

bad_number:
	cgc_put("bad number\n");
}

int
cgc_get_number(const char *str) {
	int n = 0, neg = 0;

	if (str[0] == '-') {
		neg = 1;
		str++;
	}
	if (str[0] < '0' || str[0] > '9')
		goto bad;

	n = str[0] - '0';
	for (str++; *str; str++) {
		if (*str >= '0' && *str <= '9')
			n = n * 10 + (*str) - '0';
		else
			goto bad;
	}
	if (neg)
		n = -n;

	if (first) {
		seq = n;
		first = 0;
	} else {
		if (n != seq + 1)
			goto bad;
		seq = n;
	}

	return (0);

bad:
	cgc_put("bad sequence\n");
	return (-1);
}

void
cgc_do_line(char *line) {
	const char *number = NULL;
	const char *cmd = NULL;
	const char *opt = NULL;
	int nextiscmd = 0, nextisopt = 0, cgc_len = cgc_strlen(line);

	number = line;
	for (int i = 0; i < cgc_len; i++) {
		if ((nextiscmd || nextisopt) && line[i] == '\0')
			continue;
		if (nextiscmd) {
			if (line[i] == ' ')
				continue;
			cmd = &line[i];
			nextiscmd = 0;
		}

		if (nextisopt) {
			if (line[i] == ' ')
				continue;
			opt = &line[i];
			nextisopt = 0;
			break;
		}

		if (line[i] == ' ') {
			if (cmd == NULL) {
				line[i] = '\0';
				nextiscmd = 1;
				continue;
			}
			if (opt == NULL) {
				line[i] = '\0';
				nextisopt = 1;
			}
		}
	}

	if (!cmd) {
		cgc_printf("no command?\n");
		return;
	}

	if (cgc_get_number(number))
		return;

	if (!cgc_strcmp(cmd, "create"))
		cgc_do_create(cmd, opt);
	else if (!cgc_strcmp(cmd, "delete"))
		cgc_do_delete(cmd, opt);
	else if (!cgc_strcmp(cmd, "speak"))
		cgc_do_speak(cmd, opt);
	else if (!cgc_strcmp(cmd, "rollover"))
		cgc_do_rollover(cmd, opt);
	else if (!cgc_strcmp(cmd, "debug"))
		cgc_do_debug(cmd, opt);
	else if (!cgc_strcmp(cmd, "name"))
		cgc_do_name(cmd, opt);
	else {
		cgc_put("invalid cmd\n");
	}
}

int
main() {
	char buf[1024], c;
	int nbuf = 0;
	size_t nr;

	

	for (;;) {
		if (receive(0, &c, 1, &nr))
			break;
		if (nr != 1)
			break;
		if (c == '\n') {
			buf[nbuf] = '\0';
			cgc_do_line(buf);
			cgc_memset(buf, 0, sizeof(buf));
			nbuf = 0;
		} else
			buf[nbuf++] = c;
		if (nbuf == sizeof(buf)) {
			cgc_put("you talk too much.\n");
			break;
		}
	}

	cgc_terminate(0);
}



static char hex[] = "0123456789abcdef";

void
cgc_put(char c) {
	cgc_transmit_all(1, &c, 1);
}

void
cgc_put(const char *s) {
	cgc_transmit_str(1, s);
}

void
cgc_put(void *v) {
	cgc_put((long)v);
}

void
cgc_put(int v) {
	cgc_put((long)v);
}

void
cgc_put(long v) {
	int i;

	for (i = 28; i >= 0; i -= 4)
		cgc_put(hex[(v >> i) & 0xf]);
}
