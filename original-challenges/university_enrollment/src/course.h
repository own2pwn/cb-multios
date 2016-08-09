#ifndef COURSE_H_
#define COURSE_H_
typedef struct course cgc_course_t;

#include <stdlib.h>

#include "time.h"
#include "ptrlist.h"
#include "professor.h"
#include "department.h"

struct course
{
    unsigned int course_id;
    cgc_professor_t *prof;
    cgc_department_t *dept;
    short course_num;

    short credits;
    short current_enrollment;
    short max_enrollment;
    cgc_days_t days;
    cgc_time_t start;
    cgc_time_t end;
    char *title;

    void (*print_course)(cgc_course_t *);
};


cgc_course_t *cgc_create_course(unsigned int course_id, char *prof_name, char *dept_name, short course_num,
                        short credits, short current_enrollment, short max_enrollment,
                        cgc_days_t days, cgc_time_t start, cgc_time_t end, char *title);
cgc_ptrlist_t *cgc_get_course(int course_id);

void cgc_list_courses_by_num(char *name, short course_num);
void cgc_list_courses_by_id(int course_id);

cgc_course_t *cgc_select_course_num(char *name, int course_num, cgc_size_t idx);
cgc_course_t *cgc_select_course_id(int course_id, cgc_size_t idx);

void cgc_print_course_list(cgc_ptrlist_t *courses);
int cgc_check_time_conflict(cgc_course_t *course1, cgc_course_t *course2);
void cgc_print_course_banner();

#endif
