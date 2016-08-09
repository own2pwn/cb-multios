#ifndef PROFESSOR_H_
#define PROFESSOR_H_
typedef struct professor cgc_professor_t;

#include <stdlib.h>
#include "ptrlist.h"
#include "course.h"

struct professor
{
    char *name;
    cgc_ptrlist_t *courses_taught;
};

cgc_professor_t *cgc_create_professor(char *name);
void cgc_add_course_to_professor(cgc_professor_t *prof, cgc_course_t *course);

cgc_professor_t *cgc_get_professor(char *name);
cgc_course_t *cgc_get_professor_class(cgc_professor_t *prof, cgc_size_t idx);

void cgc_list_professors();
void cgc_list_classes_taught(cgc_professor_t *prof);

#endif
