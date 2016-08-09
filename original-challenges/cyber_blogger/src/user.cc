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
#include "user.h"
#include "io.h"

cgc_User::cgc_User(char *_username, char *_pw, char *_friend_code)
{
    profile = NULL;
    cgc_memcpy(username, _username, sizeof(username));
    cgc_memcpy(pw, _pw, sizeof(pw));
    cgc_memcpy(friend_code, _friend_code, sizeof(friend_code));
    total_likes = 0;

}

cgc_User::~cgc_User()
{
    posts.cgc_clear_list(true);
    friends.cgc_clear_list(true);
    if (profile)
        delete profile;
}

cgc_BlogPost *cgc_User::cgc_get_post(int idx)
{
    if (idx < posts.cgc_length())
        return posts[idx];
    else
        return NULL;
}

char *cgc_User::cgc_get_username()
{
    return username;
}

bool cgc_User::cgc_is_full_user()
{
    //printf("f len = %d, p len = %d, tot_like = %d\n", friends.cgc_length(), posts.cgc_length(), total_likes);
    return (friends.cgc_length() >= 3 && posts.cgc_length() >= 5 && total_likes > 5);
}

bool cgc_User::cgc_check_password(char *pw_attempt)
{
    return (cgc_strcmp(pw, pw_attempt) == 0);
}

bool cgc_User::cgc_add_friend(cgc_User *new_friend, char *fc_attempt)
{
    if (new_friend == this) {
        printf("Trying to cgc_add self\n");
        return false;
    }

    if (!new_friend || cgc_strcmp(new_friend->friend_code, fc_attempt))
        return false;

    if (!friends.cgc_find(new_friend))
        return friends.cgc_add(new_friend);
    else
        return false;
}

bool cgc_User::cgc_edit_profile()
{
    char *temp_profile;
    printf("Enter new profile info (end with: ```)\n");
    temp_profile = IO::cgc_iotextdup(1024);
    if (temp_profile) {
        if (profile)
            delete[] profile;
        profile = temp_profile;
        return true;
    } else {
        return false;
    }
}

bool cgc_User::cgc_add_post()
{
    char *text = NULL;
    cgc_File *file = NULL;
    int selection = 0;
    cgc_BlogPost *post = NULL;
    printf("Enter post title: \n\t");
    if(!IO::cgc_readline(128)) {
        printf("Bad title. Try again\n");
            return false;
    } else {
        post = cgc_BlogPost::cgc_create_blog_post(this, IO::cgc_buf());
    }

    while(true) {
        printf("Enter Post Text (end with: ```):\n");
        text = IO::cgc_iotextdup(4096);
        if(text) {
            post->cgc_add_text_block(text);
        }

        if (cgc_is_full_user()) {
            printf("Upload Image?\n");
            printf("1. Yes\n");
            printf("2. No\n");
            printf("Selection: ");
            selection = IO::cgc_readnum();

            if (selection == 1) {
                file = IO::cgc_upload_file();
                if (file)
                    post->cgc_add_file(file);
            }
        }

        printf("Add more Text?\n");
        printf("1. Yes\n");
        printf("2. No\n");
        printf("Selection: ");
        selection = IO::cgc_readnum();

        if (selection == 1) {
            continue;
        }
        break;
    }

    return posts.cgc_add(post);
}

bool cgc_User::cgc_delete_post()
{
    int selection;
    printf("Select a post to delete:\n");
    printf("0. Go Back\n");
    cgc_list_posts();
    printf("Selection: ");
    selection = IO::cgc_readnum();

    if (selection == 0) {
        printf("Returning to main menu\n");
        return false;
    }

    if (selection > posts.cgc_length()) {
        printf("Bad Selection\n");
        printf("Returning to main menu\n");
        return false;
    }

    return posts.cgc_remove(--selection);
}

cgc_size_t cgc_User::cgc_num_posts()
{
    return posts.cgc_length();
}

void cgc_User::cgc_print_profile()
{
    printf("cgc_User %s's profile:\n", username);
    if (profile && cgc_strlen(profile))
        printf("%s\n", profile);
    else
        printf("No profile yet!\n");
}

void cgc_User::cgc_list_posts()
{
    for (int i = 0; i < posts.cgc_length(); i++)
        printf("%d - %s\n", i+1, posts[i]->cgc_get_title());
}

void cgc_User::cgc_print_post(cgc_size_t idx)
{
    if (idx < 1 || idx > posts.cgc_length()) {
        printf("%s's post id, %d, was not found\n", username, idx);
        return;
    }

    printf("Post by: %s\n", username);
    posts[--idx]->cgc_print_post();
}

void cgc_User::cgc_like_post(cgc_User *reg_user, cgc_size_t idx)
{
    if (!reg_user) {
        printf("Only registered users can cgc_like a post\n");
        return;
    }

    if (reg_user == this) {
        printf("Sorry, you can't cgc_like your own post...\n");
        return;
    }

    if (idx < 1 || idx > posts.cgc_length()) {
        printf("%s's post id, %d, was not found\n", username, idx);
        return;
    }

    posts[--idx]->cgc_like(reg_user);
    printf("Liked!\n");
}

void cgc_User::cgc_list_friends() {
    for (int i = 0; i < friends.cgc_length(); i++)
        printf("%d - %s\n", i+1, friends[i]->username);
}

bool cgc_User::cgc_unfriend()
{
    int selection;
    printf("Select a user to cgc_unfriend:\n");
    printf("0. Go Back\n");
    cgc_list_friends();
    printf("Selection: ");
    selection = IO::cgc_readnum();

    if (selection == 0) {
        printf("Returning to main menu\n");
        return false;
    }

    if (selection > friends.cgc_length()) {
        printf("Bad Selection\n");
        printf("Returning to main menu\n");
        return false;
    }

    return friends.cgc_remove(--selection);
}

void cgc_User::cgc_add_like()
{
    total_likes++;
}

