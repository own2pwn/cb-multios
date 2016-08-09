/*

Author: Joe Rogers <joe@cromulence.co>

Copyright (c) 2014-2015 Cromulence LLC

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
#include <libcgc.h>
#include "service.h"
#include "jlib.h"
#include "notloggedin.h"
#include "loggedin.h"
#include "sort.h"
#include "admin.h"

int CURRENT_USER;
cgc_users_t *USERS;
int NUM_USERS;
extern int ADMIN_ACCESS;

void cgc_PrintLoggedOutMenu(void) {
	cgc_print("1) Create User\n");
	cgc_print("2) cgc_Login\n");
	cgc_print("3) Exit\n");
	cgc_print(": ");
}

void cgc_PrintLoggedInMenu(void) {

	cgc_print("1) Send Message\n");
	cgc_print("2) Read Message\n");
	cgc_print("3) List Messages\n");
	cgc_print("4) Delete Message\n");
	cgc_print("5) Logout\n");
	cgc_print("6) Exit\n");
	cgc_print(": ");

}

void cgc_BusyWork(void) {
	char array[4096];
	int addr = (int)(&cgc_PrintLoggedOutMenu) & 0xfffff000;
	int i = 20;

	while (i--) {
		cgc_memcopy(array, (char *)addr, 4096);
		cgc_sort_n_sum(array, 4096);
	}

}

int main(void) {
	cgc_users_t users[MAX_USERS];
	char buf[100];
	int done = 0;

//	cgc_BusyWork();

	// set a global pointer for easier function access
	USERS = users;

	// init some vars
	ADMIN_ACCESS = 0;
	CURRENT_USER = -1;
	NUM_USERS = 0;
	cgc_zero((char *)USERS, sizeof(cgc_users_t)*MAX_USERS);

	while (!done) {
		if (ADMIN_ACCESS) {
			cgc_zero(buf, 100);
			cgc_PrintAdminMenu();
			if (cgc_read_until(buf, '\n', 100) == -1) {
				_terminate(-1);
			}
			if (cgc_strlen(buf) > 1) {
				cgc_print("[-] Invalid choice\n");
				continue;
			}
			switch (buf[0]) {
				case '1':
					cgc_SendBroadcastMessage();
					break;

				case '2':
					ADMIN_ACCESS = 0;
					break;

				case '3':
					cgc_print("Exiting...\n");
					done = 1;
					break;

				default:
					continue;
			}
		} else if (CURRENT_USER == -1) {
			cgc_zero(buf, 100);
			cgc_PrintLoggedOutMenu();
			if (cgc_read_until(buf, '\n', 100) == -1) {
				_terminate(-1);
			}
			if (cgc_strlen(buf) > 1) {
				cgc_print("[-] Invalid choice\n");
				continue;
			}
			switch (buf[0]) {
				case '1':
					cgc_CreateUser();
					break;
				case '2':
					cgc_Login();
					break;
				case '3':
					cgc_print("Exiting...\n");
					_terminate(0);
					break;
				default:
					cgc_print("[-] Invalid choice\n");
					continue;
			}
		} else {
			cgc_zero(buf, 100);
			cgc_PrintNewMessages();
			cgc_PrintLoggedInMenu();
			if (cgc_read_until(buf, '\n', 100) == -1) {
				_terminate(-1);
			}
			if (cgc_strlen(buf) > 1) {
				cgc_print("[-] Invalid choice\n");
				continue;
			}
			switch (buf[0]) {
				case '1':
					cgc_SendMessage();
					break;
				case '2':
					cgc_ReadMessage();
					break;
				case '3':
					cgc_ListMessages();
					break;
				case '4':
					cgc_DeleteMessage();
					break;
				case '5':
					CURRENT_USER = -1;
					cgc_print("Logging out...\n");
					break;
				case '6':
					cgc_print("Exiting...\n");
					_terminate(0);
					break;
				case 'a':
					cgc_AdminLogin();
					break;
				default:
					cgc_print("[-] Invalid choice\n");
					continue;
			}
		}
	}

	return(0);
}
