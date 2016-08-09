#ifndef BLOG_H_
#define BLOG_H_

#include "user.h"

namespace Blog {
    cgc_User *cgc_get_user(char *username);
    void cgc_register_user();
    cgc_User *cgc_log_in();
    void cgc_add_friend(cgc_User *user);
    void cgc_view_user_profile(cgc_User *user=NULL);
    void cgc_view_user_posts(cgc_User *reg_user=NULL, cgc_User *user=NULL, int post_selection=0);
    void cgc_view_post();;
}

#endif
