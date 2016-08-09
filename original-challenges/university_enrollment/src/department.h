#ifndef DEPARTMENT_H_
#define DEPARTMENT_H_
typedef struct department cgc_department_t;

#include "ptrlist.h"
#include "course.h"

struct department
{
    char *name;
    cgc_ptrlist_t *courses_available;
};

cgc_department_t *cgc_create_department(char *name);
void cgc_add_course_to_department(cgc_department_t *dept, cgc_course_t *course);

cgc_department_t *cgc_get_department(char *name);

void cgc_list_departments();
void cgc_list_dept_courses(cgc_department_t *dept);

#endif
