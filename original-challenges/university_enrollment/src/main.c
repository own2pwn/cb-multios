/*
 * Copyright (c) 2015 Kaprica Security, Inc.
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

#include "read.h"
#include "student.h"
#include "course.h"
#include "department.h"
#include "professor.h"
#include "data_gen.h"


static const int LINE_SIZE = 1024;
static char line[LINE_SIZE];

int cgc_yes_no_input()
{
    printf("1 - Yes\n");
    printf("2 - No\n");
    printf("Choice: ");
    return cgc_readnum(line, LINE_SIZE);
}

char *cgc_answer_question(char *question)
{
    int socket_open = 1;
    printf("%s", question);
    socket_open = cgc_readline(line, LINE_SIZE);
    while(socket_open && socket_open != -1) {
        printf("Bad Input\n");
        printf("%s", question);
    }

    return line;
}

cgc_student_t *cgc_register_student()
{
    cgc_student_t *student = NULL;
    char *answer;

    answer = cgc_answer_question("Enter name: ");
    student = cgc_create_student(answer);

    printf("Select a major\n");
    printf("List Departments?\n");
    if (cgc_yes_no_input() == 1)
        cgc_list_departments();

    answer = cgc_answer_question("Enter your Major's Department Name: ");
    if(!student->set_major(student, answer))
        printf("Bad Department Name\n");

    printf("Congratulations. You're read to start adding classes\n");
    return student;
}

void cgc_search_classes()
{
    int done = FALSE;
    int choice = 0;
    char *answer, *dept_name;
    short course_num;
    int course_id;

    while(!done) {
        printf("Class Search\n");
        printf("0 - Go Back\n");
        printf("1 - Search by Professor Name (ex: Gerardo Chacon)\n");
        printf("2 - Search by Department Name (ex: MAT)\n");
        printf("3 - Search by Class ID (ex: MAT 150)\n");
        printf("4 - Search by Course ID (ex: 2600)\n");
        printf("Choice: ");
        choice = cgc_readnum(line, LINE_SIZE);
        if (choice == -1) {
            break;
        }

        switch(choice) {
        case 0:
            done = TRUE; break;
        case 1:
            printf("List Professors?\n");
            if (cgc_yes_no_input() == 1)
                cgc_list_professors();
            answer = cgc_answer_question("Enter professor's full name: ");
            cgc_list_classes_taught(cgc_get_professor(answer));
            break;
        case 2:
            printf("List Departments?\n");
            if (cgc_yes_no_input() == 1)
                cgc_list_departments();
            answer = cgc_answer_question("Enter Department name: ");
            cgc_list_dept_courses(cgc_get_department(answer));
            break;
        case 3:
            answer = cgc_answer_question("Enter Class ID: ");
            dept_name = cgc_strsep(&answer, " ");
            if (answer) {
                course_num = cgc_strtol(answer, NULL, 10);
                cgc_list_courses_by_num(dept_name, course_num);
            } else {
                printf("Bad Input\n");
            }
            break;
        case 4:
            answer = cgc_answer_question("Enter Course ID: ");
            course_id = cgc_strtol(answer, NULL, 10);
            cgc_list_courses_by_id(course_id);
            break;
        default:
            printf("Bad Input\n");
        }
    }

    printf("Returning to main menu\n");
}

void cgc_add_class(cgc_student_t *student)
{
    int done = FALSE;
    int choice = 0;
    char *answer, dept_name[8], *pdept_name;
    short course_num;
    int course_id;
    cgc_course_t *course;

    while(!done) {
        printf("Add Class to Schedule\n");
        printf("0 - Go Back\n");
        printf("1 - Add by Class Num (ex: MAT 150)\n");
        printf("2 - Add by Course ID (ex: 2600)\n");
        printf("Choice: ");
        choice = cgc_readnum(line, LINE_SIZE);
        if (choice == -1)
            break;

        switch(choice) {
        case 0:
            done = TRUE; break;
        case 1:
            answer = cgc_answer_question("Enter Class ID: ");
            pdept_name = cgc_strsep(&answer, " ");
            if(!pdept_name || !answer || cgc_strlen(pdept_name) > 7) {
                printf("Bad Input\n");
                break;
            }
            cgc_strcpy(dept_name, pdept_name);
            course_num = cgc_strtol(answer, NULL, 10);
            printf("0 - Go Back\n");
            cgc_list_courses_by_num(dept_name, course_num);
            printf("Choice: ");
            choice = cgc_readnum(line, LINE_SIZE);
            if (choice && choice != -1) {
                course = cgc_select_course_num(dept_name, course_num, --choice);
                if (course) {
                    if (student->add_course(student, course))
                        printf("Successfully added course!\n");
                    else
                        printf("Could not add course.\n");
                } else {
                    printf("Bad selection\n");
                }
            }
            break;
        case 2:
            answer = cgc_answer_question("Enter Course ID: ");
            course_id = cgc_strtol(answer, NULL, 10);
            printf("0 - Go Back\n");
            cgc_list_courses_by_id(course_id);
            printf("Choice: ");
            choice = cgc_readnum(line, LINE_SIZE);
            if (choice && choice != -1) {
                course = cgc_select_course_id(course_id, --choice);
                if (course) {
                    if (student->add_course(student, course))
                        printf("Successfully added course!\n");
                    else
                        printf("Could not add course.\n");
                } else {
                    printf("Bad selection\n");
                }
            }
            break;
        default:
            printf("Bad Input\n");
        }
    }

    printf("Returning to main menu\n");
}

void cgc_remove_class(cgc_student_t *student)
{
    int choice = 0;

    printf("Remove Class from Schedule\n");
    printf("0 - Go Back\n");
    student->list_classes(student);
    printf("Choice: ");
    choice = cgc_readnum(line, LINE_SIZE);
    if (choice && choice != -1) {
        if (student->remove_course(student, --choice))
            printf("Successfully removed course!\n");
        else
            printf("Bad selection\n");
    }

    printf("Returning to main menu\n");
}

int main()
{
    cgc_generate_data();
    printf("University Course Scheduler\n\n");
    cgc_student_t *student = cgc_register_student();
    int exitted = FALSE;
    int choice = 0;

    while (!exitted) {
        printf("Main Menu\n");
        printf("1 - Search Classes\n");
        printf("2 - Add Class\n");
        printf("3 - Remove Class\n");
        printf("4 - Print Schedule\n");
        printf("5 - Exit\n");
        printf("Choice: ");
        choice = cgc_readnum(line, LINE_SIZE);
        if (choice == -1)
            break;

        switch(choice) {
        case 1:
            cgc_search_classes(); break;
        case 2:
            cgc_add_class(student); break;
        case 3:
            cgc_remove_class(student); break;
        case 4:
            student->print_schedule(student); break;
        case 5:
            exitted = TRUE; break;
        default:
            printf("Bad Input\n");
        }
    }

    printf("Exiting.\n");
    return 0;
}
