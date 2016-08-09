/*

Author: Debbie Nuttall <debbie@cromulence.com>

Copyright (c) 2014 Cromulence LLC

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#ifndef TIMECARD_H
#define TIMECARD_H

#define WEEKS_IN_A_YEAR 52
#define EMPLOYEE_NAME_LEN 64
#define NUMBER_OF_EMPLOYEES 50
#define PAYROLL_TAX_RATE .0765

typedef struct money_s
{
	int dollars;
	int cents;
} cgc_money, *cgc_pmoney;

typedef struct time_s
{
	int hours;
	int minutes;
} cgc_time, *cgc_ptime;

// A function pointer will be used to calculate overtime differently for exempt vs non-exempt employees 
typedef void (*cgc_overtime_calc)(cgc_pmoney, cgc_pmoney, cgc_ptime);

// A cgc_payroll struct is used to hold cgc_time and pay information for one week
typedef struct payroll_s{
	cgc_time 	standardtime;
	cgc_time 	overtime;
	cgc_money 	standardpay;
	cgc_money 	overtimepay;
	cgc_money 	payroll_tax;
#ifdef PATCHED
	char paycheck[20];
#else
	char paycheck[12];
#endif
	cgc_overtime_calc calculate_overtime;
} cgc_payroll, *cgc_ppayroll;

// The cgc_employee structure holds various cgc_employee information as well as a cgc_payroll
// record for each week of the year. 
typedef struct employee_s{
	char 	name[EMPLOYEE_NAME_LEN];
	int 	id;
	cgc_money 	wage;
	int 	exempt;
	cgc_payroll paychecks[WEEKS_IN_A_YEAR];
} cgc_employee, *cgc_pemployee;

void cgc_atom(cgc_pmoney amount, char *str);
void cgc_mtoa(char *str, cgc_pmoney amount);
void cgc_atoh(cgc_ptime t, char *str);
void cgc_htoa(char *str, cgc_ptime t);
void cgc_initialize_employee(cgc_pemployee empl);
void cgc_add_time(cgc_ptime t, int hours, int minutes);
void cgc_add_money(cgc_pmoney dest, float cgc_money);
void cgc_add_pay(cgc_pmoney pay, cgc_pmoney rate, cgc_ptime timeworked);
void cgc_log_hours(cgc_ppayroll paycheck, char *hours);
void cgc_log_overtime_hours(cgc_ppayroll paycheck, char *hours);
void cgc_calculate_standardpay(cgc_pmoney pay, cgc_pmoney wage, cgc_ptime timeworked);
void cgc_calculate_totalpay(cgc_ppayroll paycheck);
void cgc_exempt_overtime(cgc_pmoney pay, cgc_pmoney wage, cgc_ptime timeworked);
void cgc_nonexempt_overtime(cgc_pmoney pay, cgc_pmoney wage, cgc_ptime timeworked);
int cgc_get_key_value(char *inbuf, size_t length, char **key, char **value);
void cgc_process_key_value(cgc_pemployee empl, char *key, char *value, int *week);
void cgc_merge_employee_records(cgc_pemployee empl, cgc_pemployee temp);
void cgc_process_query(int query, cgc_employee employee_list[], cgc_pemployee temp, int week);
void cgc_output_paycheck(cgc_pemployee empl, int week);

// Read status codes
#define READ_ERROR 			-1
#define NEWLINE_RECEIVED 	 1
#define KEY_VALUE_RECEIVED	 2
#define OTHER_INPUT_RECEIVED 3

// Query codes
#define QUERY_NONE		0
#define QUERY_ONE		1
#define QUERY_ALL		2
#define QUERY_WEEK		3
#define QUERY_WEEK_ALL 	4

#endif //TIMECARD_H
