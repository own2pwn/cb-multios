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
#include "blog.h"
#include "io.h"
#include "list.h"

extern "C" {
#include <string.h>
};

static cgc_List<cgc_User *> g_reg_users;

cgc_User *Blog::cgc_get_user(char *username)
{
    for (int i=0; i < g_reg_users.cgc_length(); i++) {
        if(cgc_strcasecmp(username, g_reg_users[i]->cgc_get_username()) == 0)
            return g_reg_users[i];
    }

    return NULL;
}

void Blog::cgc_register_user()
{
    char username[16];
    char pw[16];
    char friend_code[16];
    cgc_User *user;

    printf("Enter username: ");
    if (IO::cgc_readline()) {
        if (cgc_strlen(IO::cgc_buf()) > 2 && cgc_strlen(IO::cgc_buf()) < 16) {
            cgc_strcpy(username, IO::cgc_buf());
        } else {
            printf("Error: Username must be between 3 and 15 characters\n");
            return;
        }
    } else {
        printf("Bad Input\n");
        return;
    }
    if (cgc_get_user(username) != NULL) {
        printf("Error: Username already exists. Try a different username\n");
        return;
    }

    while(true) {
        printf("Enter password: ");
        if (IO::cgc_readline()) {
            if (cgc_strlen(IO::cgc_buf()) > 4 && cgc_strlen(IO::cgc_buf()) < 16) {
                cgc_strcpy(pw, IO::cgc_buf());
                break;
            } else {
                printf("Bad Password\n");
            }
        } else {
            printf("Bad Input\n");
        }
    }

    while(true) {
        printf("Friend code: ");
        if (IO::cgc_readline()) {
            if (cgc_strlen(IO::cgc_buf()) > 4 && cgc_strlen(IO::cgc_buf()) < 16) {
                cgc_strcpy(friend_code, IO::cgc_buf());
                break;
            } else {
                printf("Bad Friend Code\n");
            }
        } else {
            printf("Bad Input\n");
        }
    }

    user = new cgc_User(username, pw, friend_code);
    g_reg_users.cgc_add(user);
}

cgc_User *Blog::cgc_log_in()
{
    char username[16];
    char pw[16];
    cgc_User *user = NULL;
    printf("Username: ");
    if (IO::cgc_readline()) {
        if (cgc_strlen(IO::cgc_buf()) > 4 && cgc_strlen(IO::cgc_buf()) < 16)
            cgc_strcpy(username, IO::cgc_buf());
    } else {
        printf("Bad input\n");
        return NULL;
    }

    printf("Password: ");
    if (IO::cgc_readline()) {
        if (cgc_strlen(IO::cgc_buf()) > 4 && cgc_strlen(IO::cgc_buf()) < 16)
            cgc_strcpy(pw, IO::cgc_buf());
    } else {
        printf("Bad input\n");
        return NULL;
    }

    user = cgc_get_user(username);
    if (user && user->cgc_check_password(pw)) {
        printf("Authenticated\n");
        return user;
    } else {
        printf("Bad username/password combination\n");
        return NULL;
    }
}

void Blog::cgc_add_friend(cgc_User *user)
{
    char username[16];
    char friend_code[16];
    cgc_User *new_friend = NULL;

    if (!user) {
        printf("Bad user\n");
        return;
    }

    printf("Enter friend username: ");
    if (IO::cgc_readline()) {
        if (cgc_strlen(IO::cgc_buf()) > 2 && cgc_strlen(IO::cgc_buf()) < 16) {
            cgc_strcpy(username, IO::cgc_buf());
            new_friend = cgc_get_user(username);
        } else {
            printf("Error: Username must be between 3 and 15 characters\n");
            return;
        }
    }

    if (new_friend == NULL) {
        printf("Error: Couldn't get friend\n");
        return;
    }

    while(true) {
        printf("Friend code: ");
        if (IO::cgc_readline()) {
            if (cgc_strlen(IO::cgc_buf()) > 4 && cgc_strlen(IO::cgc_buf()) < 16) {
                cgc_strcpy(friend_code, IO::cgc_buf());
                break;
            } else {
                printf("Bad Friend Code\n");
            }
        } else {
            printf("Bad Input\n");
        }
    }

    if (user->cgc_add_friend(new_friend, friend_code))
        printf("Added friend, %s!\n", username);
    else
        printf("Couldn't cgc_add as friend.\n");
}


void Blog::cgc_view_user_profile(cgc_User *user)
{
    if (!user) {
        printf("Enter username: ");
        if (IO::cgc_readline()) {
            if (cgc_strlen(IO::cgc_buf()) > 2 && cgc_strlen(IO::cgc_buf()) < 16) {
                user = cgc_get_user(IO::cgc_buf());
            }
        }

        if (!user) {
            printf("Bad Username\n");
            return;
        }
    }

    user->cgc_print_profile();
}

void Blog::cgc_view_user_posts(cgc_User *reg_user, cgc_User *user, int post_selection)
{
    if (!user) {
        printf("Enter username: ");
        if (IO::cgc_readline()) {
            if (cgc_strlen(IO::cgc_buf()) > 2 && cgc_strlen(IO::cgc_buf()) < 16) {
                user = cgc_get_user(IO::cgc_buf());
            }
        }

        if (!user) {
            printf("Bad Username\n");
            return;
        }
    }

    if (!post_selection) {
        printf("Select a post id to view\n");
        printf("0. Go Back\n");
        user->cgc_list_posts();
        printf("Selection: ");
        post_selection = IO::cgc_readnum();
        if (post_selection == 0) {
            printf("Returning to main menu\n");
            return;
        }
    }

    if( post_selection > user->cgc_num_posts()) {
        printf("Bad Post Selection\n");
        return;
    }

    user->cgc_print_post(post_selection);
    if (reg_user && reg_user != user) {
        int like_selection = 0;
        printf("Like Post?\n");
        printf("1. Yes\n");
        printf("2. No\n");
        printf("Selection: ");
        like_selection = IO::cgc_readnum();
        if (like_selection == 1) {
            user->cgc_like_post(reg_user, post_selection);
        }
    }
}
