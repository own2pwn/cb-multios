/*

Author: Steve Wood <swood@cromulence.com>

Copyright (c) 2016 Cromulence LLC

Permission is hereby granted, cgc_free of charge, to any person obtaining a copy
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
#include "stdlib.h"
#include "service.h"
#include "filesystem.h"
#include "stdio.h"
#include "string.h"
#include "input.h"
#include "malloc.h"


extern cgc_fileHandleType securityIDFileHandle;

int main(void) {

int retcode;
unsigned int count;
cgc_fileHandleType fh;
int useraccount;
void *message;
int messagetype;
cgc_newUserMessageType *newuser;
cgc_loginMessageType *loginuser;
cgc_newPostMessageType *newPost;
cgc_addCommentMessageType *newComment;
unsigned int postID;
int responseCode;
unsigned int nextPostID = 100;
unsigned int sessionToken;
char *postText;
int postSize;
int endIt;
    
    // using a small blocksize of 256 bytes because postings are small and this is more efficient
    retcode = cgc_initFileSystem(512, 512, 512*2000);

    securityIDFileHandle = -1;

    if (retcode != 0) {

        cgc_printf("Error making filesystem.\n");
        _terminate(-1);
    }

    retcode = cgc_makeMemoryFile("sticky.posts", 0x4347C000 + 1536, 160*16,  1,  ROOT_ID );

    if ( retcode != 0 ) {

        cgc_printf("Error making posts.log\n");
        _terminate(-1);
    }

    retcode = cgc_makeMemoryFile("initialPostID.mem", 0x4347C000, 4, 1, ROOT_ID );

    if ( retcode != 0 ) {

        cgc_printf("Error making posts.log\n");
        _terminate(-1);
    }

	retcode = cgc_createFile("Users.db", REGULAR, ROOT_ID);	
    
    if ( retcode != 0 ) {

        cgc_printf("Error making Users.db\n");
        _terminate(-1);
    }

    retcode = cgc_createFile("posts.log", REGULAR, ROOT_ID);

    if ( retcode != 0 ) {

        cgc_printf("Error making posts.log\n");
        _terminate(-1);
    }

    // seed the first postID with magic page data.  After this they just increase by 1 each time
    fh = cgc_openFile("initialPostID.mem", ROOT_ID);

    if ( fh < 0 ) {

        cgc_printf("Error opening initialPostID.mem\n");
        _terminate(-1);
    }
    cgc_readFile(fh, (void *)&nextPostID, sizeof(nextPostID), 0, 0, ROOT_ID);
    nextPostID &= 0x0fffffff;

    // we'll never re-seed the postID so just delete the file
    cgc_deleteFile(fh, ROOT_ID);

    // this file will allow us to get semi-random userID's from the magic page.  It is kept open and a new ID is 
    // read whenever a new account is created
    retcode = cgc_makeMemoryFile("UserIDs.mem", 0x4347C004, 1532, 1, ROOT_ID );

    if ( retcode != 0 ) {

        cgc_printf("Error making UserIDs.mem\n");
        _terminate(-1);
    }

    retcode = allocate(1024, 0, &message);

    if (retcode != 0) {

        _terminate(-1);
    }

    endIt = 0;

    while (!endIt) {

    	messagetype = cgc_receiveMessage(message);

    	switch (messagetype) {

    			// add a new user
    		case 0xa0:

    			newuser = (cgc_newUserMessageType *)message;

    			if (cgc_create_user(newuser->name, newuser->password, newuser->fullname) >= 0 ) {

    				responseCode = 0;
    				
    			}
    			else {

    				responseCode = -1;

    			}

    			cgc_sendResponse((void *)&responseCode, sizeof(responseCode));

    			break;

    			// cgc_authenticate a user
    		case 0xb0:

    			loginuser = (cgc_loginMessageType *)message;
    			useraccount = cgc_authenticate(loginuser->name, loginuser->password);

    			cgc_sendResponse((void *)&useraccount, sizeof(useraccount));

    			break;

    			// retrieve a single post to this user's feed
    		case 0xc0:

                sessionToken = *(unsigned int *)message;

                retcode = cgc_newFeedPost(sessionToken, &postText, &postSize);

                if (retcode == 0 ) {

                    cgc_sendResponse((void *)postText, postSize);
                    deallocate((void *)postText, postSize);
                }
                else {

                    retcode = -1;
                    cgc_sendResponse((void *)&retcode, sizeof(retcode));

                }
    			break;

    			 // record a new post from the user
    		case 0xd0:

                newPost = (cgc_newPostMessageType *)message;

                retcode = cgc_savePost(nextPostID, newPost->sessionToken, newPost->post);

                if (retcode == 0) {

                    retcode = nextPostID;
                    ++nextPostID;
                }
                else {

                    retcode = -1;
                }

                cgc_sendResponse((void *)&retcode, sizeof(retcode));

    			break;

                // comment on a post
            case 0xe0:

                newComment = (cgc_addCommentMessageType *)message;

                retcode = cgc_saveComment(newComment->postID, newComment->commenterID, newComment->comment);

                cgc_sendResponse((void *)&retcode, sizeof(retcode));

                break;

                // retrieve a specific post by its ID--this will include any comments as well
            case 0xf0:

                postID = *(unsigned int *)message;

                if ( postID < 16 ) {

                     retcode = cgc_sendStickPost( postID );
                }
                else {

                    retcode = cgc_retrievePost( postID, 1 , &postText, &postSize);

                    if (retcode == 0) {

                        cgc_sendResponse((void *)postText, postSize);
                        deallocate((void *)postText, postSize);
                    }
                }

                if ( retcode == -1 ) {

                    cgc_sendResponse((void *)&retcode, sizeof(retcode));

                }
                // response sent by the cgc_retrievePost() function
                break;

            case 100:

                endIt = 1;
                break;

    		default:

                endIt = 1;

    			break;

    	} //switch


    } // while (1)

    cgc_printf("BYE!\n");

}  // main  

