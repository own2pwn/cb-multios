#ifndef STUDENT_H_
#define STUDENT_H_

#define MAX_COURSES 7
#define MAX_CREDITS 17
#define MAJOR_COURSE_MAX 4
typedef struct student cgc_student_t;

#include "course.h"
#include "department.h"

enum {FALSE=0, TRUE=1};

struct student
{
    char *name;
    cgc_department_t *major;

    int num_courses;
    int num_major_courses;
    int total_credits;
    cgc_course_t *courses[MAX_COURSES];
    int (*add_course)(cgc_student_t *, cgc_course_t *);
    int (*set_major)(cgc_student_t *, char *);
    int (*remove_course)(cgc_student_t *, size_t);
    void (*list_classes)(cgc_student_t *);
    void (*print_schedule)(cgc_student_t *);
};

cgc_student_t *cgc_create_student();
void cgc_list_added_courses(cgc_student_t *student);

#endif
