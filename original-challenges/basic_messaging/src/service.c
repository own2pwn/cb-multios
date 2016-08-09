#include <libcgc.h>
#include <stdlib.h>

/*

Author: John Berry <john.n.berry@gmail.com>

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

/* Service that allows people to post messages to each other
 * Create User
 * Delete User
 * Login
 * Logout
 * Read Message
 * Send Message
 * List Messages
 * Delete Message
 *
*/

#define MESSAGE_LENGTH 0x20
#define USERNAME_LENGTH 0x10

/**
 * Structure to store an individual cgc_message and necessary metadata
 **/
typedef struct cgc_message {
	struct cgc_message *next;
	unsigned int message_id;
	unsigned int read;
	char cgc_message[MESSAGE_LENGTH];
} cgc_message, *cgc_pmessage;

/**
 * Structure to manage the messages for a single cgc_user
 **/
typedef struct cgc_message_manager {
	cgc_pmessage root;
	unsigned int message_count;
} cgc_message_manager, *cgc_pmessage_manager;

/**
 * Structure with necessary data for an individual cgc_user.
 **/
typedef struct cgc_user {
	char name[USERNAME_LENGTH];
	cgc_pmessage_manager pmm;	
	struct cgc_user *next;
} cgc_user, *cgc_puser;

/**
 * Structure to manage all users. Only one instance of this per application execution.
 **/
typedef struct cgc_user_manager {
	cgc_puser root;
	unsigned int user_count;
} cgc_user_manager, *cgc_puser_manager;

cgc_pmessage_manager cgc_init_manager( void );

/**
 * Lists all existing users. This functions is for debugging purposes only. Normal
 *   execution will never call here.
 * @param pum List all existing usernames.
 * @return Returns nothing.
 **/
void cgc_list_users( cgc_puser_manager pum )
{
	cgc_puser walker = NULL;

	if ( pum == NULL ) {
		goto end;
	}

	walker = pum->root;

	while ( walker ) {
		cgc_puts(walker->name);
		walker = walker->next;
	}
end:
	return;
}

/**
 * Initialize the global cgc_user management structure. This should only be called once
 * @return Returns the cgc_user management structure for a give execution or NULL on error.
 **/
cgc_puser_manager cgc_init_users( void )
{
	cgc_puser_manager pum = NULL;

	if ( allocate( sizeof(cgc_user_manager), 0, (void**)&pum ) != 0) {
		cgc_puts( "[-] Error Failed to allocate buffer\n" );
		goto end;
	}

	pum->root = NULL;
	pum->user_count = 0;
end:
	return pum;
}

/**
 * Creates a new cgc_user and adds it to the cgc_user management structure.
 * @param pum A pointer to the global cgc_user management structure.
 * @param username Pointer to an ascii username.
 * @return Returns the newly created cgc_user id or 0 on failure.
 **/
cgc_size_t cgc_create_user( cgc_puser_manager pum, char *username )
{
	cgc_size_t id = 0;
	cgc_puser walker = NULL;
	cgc_puser newuser = NULL;

	if ( pum == NULL ) {
		goto end;
	}

	if ( username == NULL ) {
		goto end;
	}

	if ( cgc_strlen(username) > USERNAME_LENGTH - 1 ) {
		cgc_puts("[-] Error username too long\n");
		goto end;
	}

	// Check for existing username
	walker = pum->root;

	while ( walker ) {
		if ( cgc_strcmp( walker->name, username ) == 0) {
			cgc_puts("[-] Error cgc_user exists\n");
			goto end;
		}
		walker = walker->next;
	}

	// Create cgc_user stucture
	if ( allocate( sizeof(cgc_user), 0, (void**)&newuser) != 0 ) {
		cgc_puts("[-] Error allocating cgc_user structure\n");
		goto end;
	}
	
	newuser->next = NULL;
	newuser->name[0] = 0x00;
	newuser->pmm = cgc_init_manager();

	cgc_strncat( newuser->name, username, USERNAME_LENGTH );

	// Check root cgc_user first
	if ( pum->root == NULL ) {
		pum->root = newuser;
		pum->user_count++;
		id = pum->user_count;
		goto end;
	}

	// Link in new cgc_user
	newuser->next = pum->root;
	pum->root = newuser;

	pum->user_count++;
	id = pum->user_count;
	
end:
	return id;
}

/**
 * Initializes the cgc_message manager structure. This will be called once per new cgc_user.
 * @return Returns the new cgc_message manager structure or NULL on failure.
 **/
cgc_pmessage_manager cgc_init_manager( void )
{
	cgc_pmessage_manager pmm = NULL;

	if ( allocate( sizeof(cgc_message_manager), 0, (void**)&pmm ) != 0) {
		cgc_puts( "[-] Error Failed to allocate buffer\n" );
		goto end;
	}

	pmm->root = NULL;
	pmm->message_count = 0;
end:
	return pmm;
}

/**
 * Adds a new cgc_message to a users cgc_message manager.
 * @param pman Pointer to a users cgc_message management structure.
 * @param pm Pointer to the cgc_message to add
 * @return Id of the new cgc_message or 0 on failure.
 **/
cgc_size_t cgc_add_message( cgc_pmessage_manager pman, cgc_pmessage pm )
{
	cgc_size_t id = 0;
	cgc_pmessage walker = NULL;

	if ( pm == NULL ) {
		goto end;
	}

	if ( pman == NULL ) {
		goto end;
	}

	// If there are no additional messages, this one goes
	//	on the root
	if ( pman->root == NULL ) {
		pman->root = pm;
		pman->message_count++;
		pm->message_id = pman->message_count;
		id = pm->message_id;
		goto end;
	}

	walker = pman->root;

	while ( walker->next != NULL ) {
		walker = walker->next;
	}

	walker->next = pm;
	pman->message_count++;
	pm->message_id = pman->message_count;
	id = pm->message_id;

end:
	return id;
}

/**
 * Creates a new cgc_message with the given text.
 * @param msg_string String containing the data to store in the cgc_message.
 * @return Returns a cgc_message structure that must be added to the cgc_message manager or NULL on failure.
 **/
cgc_pmessage cgc_create_message( char * msg_string )
{
	cgc_pmessage pmsg = NULL;
	int counter = 0;

	if ( msg_string == NULL ) {
		goto end;
	}

	if ( cgc_strlen( msg_string ) > MESSAGE_LENGTH - 1 ) {
		cgc_puts("[-] Error Message too long\n");
		goto end;
	}

	if ( allocate( sizeof(cgc_message), 0, (void**)&pmsg)  != 0 ) {
		cgc_puts("[-] Error Failed to allocate cgc_message struct\n");
		goto end;
	}

	pmsg->next = NULL;
	pmsg->message_id = 0;
	pmsg->read = 0;

	cgc_bzero( pmsg->cgc_message, MESSAGE_LENGTH );

	for ( counter = 0; counter < cgc_strlen( msg_string ); counter++ ) {
		pmsg->cgc_message[counter] = msg_string[counter];
	}

end:
	return pmsg;
}

/**
 * Deletes a cgc_message with a given id from a users cgc_message list.
 * @param pmm Pointer to a users cgc_message management structure.
 * @param message_id Id of the cgc_message to be deleted.
 * @return Only returns void. Any errors are displayed to the cgc_user.
 **/
void cgc_delete_message( cgc_pmessage_manager pmm, cgc_size_t message_id )
{
	cgc_pmessage walker = NULL;
	cgc_pmessage prev = NULL;

	if ( pmm == NULL ) {
		goto end;
	}

	if ( message_id > pmm->message_count ) {
		cgc_puts( "[-] Message ID out of range\n");
		goto end;
	}

	walker = pmm->root;

	while ( walker ) {
		if ( walker->message_id == message_id ) {
			// If prev is null that means the root cgc_message
			//   is the one to be deleted
			if ( prev == NULL ) {
				pmm->root = walker->next;
			} else {
				prev->next = walker->next;
			}

			deallocate( walker, sizeof( cgc_message ) );
			goto end;
		}

		prev = walker;
		walker = walker->next;
	}

	if ( walker == NULL ) {
		cgc_puts("[-] Message ID not found\n");
	}
end:
	return;
}

/**
 * Displays a single cgc_message to a cgc_user
 * @param pmm Pointer to the cgc_message manager structure of a cgc_user.
 * @param message_id Id of the cgc_message to be displayed.
 **/
void cgc_read_message( cgc_pmessage_manager pmm, cgc_size_t message_id )
{
	cgc_size_t retval = 0;
	cgc_pmessage walker = NULL;
	char buffer[0x100];

	if ( pmm == NULL ) {
		goto end;
	}

	if ( message_id > pmm->message_count ) {
		cgc_puts( "[-] Message ID out of range\n");
		goto end;
	}

	cgc_bzero( buffer, 0x100 );
	cgc_strncat( buffer, "***********************************\n", 0x100 );
	
	walker = pmm->root;

	while ( walker ) {
		if ( walker->message_id == message_id ) {
			retval = cgc_strlen(buffer);
			cgc_itoa( buffer + retval, message_id, 0x100-retval );
			cgc_strncat(buffer, ":  ", 0x100 );
			cgc_strncat(buffer, walker->cgc_message, 0x100 );
			cgc_strncat( buffer, "\n***********************************\n", 0x100 );
			cgc_puts( buffer );
			walker->read = 1;
			goto end;
		}

		walker = walker->next;
	}

	if ( walker == NULL ) {
		cgc_puts("[-] Message ID not found\n");
	}

end:
	return;
}

/**
 * Lists all unread messages of a cgc_user. This function is where the bug is. The variable
 *  used to store the number of unread messages is an unsigned char so if more than 255
 *  messages are waiting to be displayed this counter will overflow. The counter is used
 *  to calculate the size of the buffer necessary to display the messages. A counter
 *  that has overflowed will result in the allocation of a buffer that is too small to
 *  contain the entire stream of messages.
 * @param pmm A pointer to the cgc_message manager of a cgc_user
 **/
void cgc_list_unread_messages( cgc_pmessage_manager pmm )
{
	cgc_pmessage walker = NULL;
	cgc_size_t size = 0;
#ifdef PATCHED
	unsigned int count = 0;
#else
	unsigned char count = 0;
#endif

	if ( pmm == NULL ) {
		return;
	}
	walker = pmm->root;

	while ( walker ) {
		if ( walker->read == 0 ) {
			count++;
		}
		walker= walker->next;
	}

	if ( count == 0 ) {
		return;
	}

	cgc_puts("Unread messages:\n");

	// Calculate size
	// Message Text
	size = count * MESSAGE_LENGTH;
	// Message border
	size += count * (72);
	// Message id and trailing newline
	// "###:  "
	size += count * 8;

	char data[size];

	cgc_bzero( data, size );

	walker = pmm->root;

	while ( walker ) {
		if ( walker->read == 0 ) {

			// Mark the cgc_message as read
			walker->read = 1;
			cgc_strcat( data, "***********************************\n");
			cgc_itoa( data + cgc_strlen(data), walker->message_id, 4 );
			cgc_strcat( data, ":  " );
			cgc_strcat( data, walker->cgc_message );
			cgc_strcat( data, "\n");
			cgc_strcat( data, "***********************************\n");
		}
		walker = walker->next;
	}

	cgc_puts( data );
end:
	return;
}

/**
 * List all existing messages of a cgc_user including those already read.
 * @param pmm Pointer to the cgc_message management structure of a cgc_user.
 **/
void cgc_list_messages( cgc_pmessage_manager pmm )
{
	cgc_pmessage walker = NULL;

	if ( pmm == NULL ) {
		goto end;
	}

	walker = pmm->root;

	while ( walker ) {
		cgc_read_message( pmm, walker->message_id );
		walker = walker->next;
	}
end:
	return;
}

/**
 * Retrieve a specific cgc_user structure from the manager
 * @param pum Pointer to the global cgc_user management structure
 * @param username Pointer to the username of the cgc_user to locate
 * @return Returns a pointer to the requested cgc_user management structure or NULL on failure.
 **/
cgc_puser cgc_get_user( cgc_puser_manager pum, char *username )
{
	cgc_puser pu = NULL;

	if ( pum == NULL || username == NULL ) {
		goto end;
	}

	pu = pum->root;

	while ( pu ) {
		if ( cgc_strcmp( pu->name, username ) == 0 ) {
			goto end;
		}
		pu = pu->next;
	}
end:
	return pu;
}

/**
 * Stores a cgc_message for a specific cgc_user
 * @param pum Pointer to the global cgc_user management structure
 * @param username Username of the cgc_user to send the cgc_message to
 * @param msg Pointer to the cgc_message to send to the cgc_user.
 * @return Returns the id of the sent cgc_message or 0 on failure.
 **/
cgc_size_t cgc_send_user_message( cgc_puser_manager pum, char *username, char *msg )
{
	cgc_size_t id = 0;
	cgc_puser pu = NULL;
	cgc_pmessage pmsg = NULL;

	if ( pum == NULL || username == NULL || msg == NULL ) {
		goto end;
	}

	pu = cgc_get_user( pum, username );

	if ( pu == NULL ) {
		cgc_puts("[-] Error invalid cgc_user\n");
		goto end;
	}

	pmsg = cgc_create_message( msg );

	if ( pmsg == NULL ) {
		cgc_puts("[-] Failed to create cgc_message\n");
		goto end;
	}

	if ( (id = cgc_add_message( pu->pmm, pmsg )) == 0 ) {
		cgc_puts("[-] Failed to add cgc_message\n");
		goto end;
	}	
end:
	return id;
}

/**
 * Handles the sub menu for a logged in cgc_user
 * @param pum Pointer to the global cgc_user management structure
 * @param pu Pointer to the cgc_user structure for the logged in cgc_user
 **/
void cgc_handle_loggedin( cgc_puser_manager pum, cgc_puser pu )
{
	cgc_size_t choice = 0;
	char cgc_message[MESSAGE_LENGTH];
	char username[USERNAME_LENGTH];
	
	if ( pum == NULL || pu == NULL ) {
		goto end;
	}

	while ( 1 ) {
		cgc_list_unread_messages( pu->pmm );

		cgc_puts("1) Send Message\n");
		cgc_puts("2) Read Message\n");
		cgc_puts("3) List Messages\n");
		cgc_puts("4) Delete Message\n");
		cgc_puts("5) Logout\n");
		cgc_puts("6) Exit\n");
		cgc_puts(": ");

		choice = 0;

		if ( cgc_receive_until( (char*)&choice, 0x0a, 2 ) == 0 ) {
			cgc_puts("[-] Receive failed\n");
			_terminate(0);
		}

		choice -= 0x30;

		if ( choice > 6 || choice < 1 ) {
			cgc_puts("[-] Invalid choice\n");
			continue;
		}

		if ( choice == 6 ) {
			cgc_puts("Exiting...\n");
			_terminate(0);
		} else if ( choice == 5 ) {
			cgc_puts("Logging out...\n");
			goto end;
		} else if ( choice == 4 ) {
			cgc_puts("ID: ");
			cgc_bzero( cgc_message, MESSAGE_LENGTH);
			cgc_receive_until( cgc_message, '\n', 4 );
			choice = cgc_atoi( cgc_message );
			cgc_delete_message( pu->pmm, choice );
		} else if ( choice == 3 ) {
			cgc_list_messages( pu->pmm );
		} else if ( choice == 2 ) {
			cgc_puts("ID: ");
			cgc_bzero( cgc_message, MESSAGE_LENGTH );
			cgc_receive_until( cgc_message, '\n', 4 );
			choice = cgc_atoi( cgc_message );
			cgc_read_message( pu->pmm, choice );
		} else if ( choice == 1 ) {
			cgc_bzero( cgc_message, MESSAGE_LENGTH );
			cgc_bzero( username, USERNAME_LENGTH );

			cgc_puts("To: ");
			cgc_receive_until( username, '\n', USERNAME_LENGTH-1);
			cgc_puts("Message: ");
			cgc_receive_until( cgc_message, '\n', MESSAGE_LENGTH-1);
			cgc_send_user_message( pum, username, cgc_message );
		}
	}

end:
	return;
}

/**
 * Function to handle the main cgc_user interface loop
 **/
void cgc_handle_menu ( )
{
	cgc_puser_manager pum = NULL;
	cgc_puser current_user = NULL;
	cgc_size_t choice = 0;
	cgc_size_t received = 0;
	char username[USERNAME_LENGTH];

	pum = cgc_init_users( );

	if ( pum == NULL ) {
		goto end;
	}

	while ( 1 ) {
		choice = 0;

		cgc_puts("1) Create User\n");
		cgc_puts("2) Login\n");
		cgc_puts("3) Exit\n");
		cgc_puts(": ");

		if ( cgc_receive_until( (char*)&choice, '\n', 2 ) == 0 ) {
			cgc_puts("[-] Receive Failed\n");
			_terminate(0);
		}

		choice -= 0x30;

		if ( choice > 3 || choice < 1 ) {
			cgc_puts("[-] Invalid choice\n");
			continue;
		}

		cgc_bzero( username, USERNAME_LENGTH );

		if ( choice == 3 ) {
			cgc_puts("Exiting...\n");
			_terminate( 0 );
		} else if ( choice == 1 ) {
			cgc_puts("username: ");
			cgc_receive_until( username, '\n', USERNAME_LENGTH - 1);
			cgc_create_user( pum, username );
		} else if ( choice == 2 ) {
			cgc_puts("username: ");
			cgc_receive_until( username, '\n', USERNAME_LENGTH - 1);
			current_user = cgc_get_user( pum, username );
			if ( current_user != NULL ) {
				cgc_puts("Login Success\n");
				cgc_handle_loggedin( pum, current_user );
				current_user = NULL;
			} else {
				cgc_puts("Login Failed\n");
			}
		}
	}

end:
	return;
}

/**
 * Main function just calls cgc_handle_menu
 * @return Always returns 0
 **/
int main(void) {
	cgc_handle_menu();
	return 0;
}
