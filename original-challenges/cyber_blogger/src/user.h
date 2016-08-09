#ifndef USER_H_
#define USER_H_

#include "list.h"
#include "blogpost.h"

class cgc_BlogPost;

class cgc_User
{
private:
    char username[16];
    char pw[16];
    char friend_code[16];
    char *profile;
    unsigned int total_likes;

    cgc_List<cgc_BlogPost *> posts;
    cgc_List<cgc_User *> friends;
public:
    cgc_User(char *_username, char *_pw, char *_friend_code);
    ~cgc_User();

    cgc_BlogPost *cgc_get_post(int idx);
    char *cgc_get_username();
    bool cgc_is_full_user(); //more than 2 posts and more than 10 likes
    bool cgc_check_password(char *pw_attempt);
    bool cgc_add_friend(cgc_User *user, char *fcode_attempt);
    bool cgc_edit_profile();
    bool cgc_add_post();
    bool cgc_delete_post();
    cgc_size_t cgc_num_posts();

    void cgc_print_profile();
    void cgc_list_posts();
    void cgc_print_post(cgc_size_t idx);
    void cgc_like_post(cgc_User *reg_user, cgc_size_t idx);
    void cgc_list_friends();
    bool cgc_unfriend();
    void cgc_add_like();
};

#endif
