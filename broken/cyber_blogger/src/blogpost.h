#ifndef BLOGPOST_H_
#define BLOGPOST_H_

#include "list.h"
#include "file.h"
#include "user.h"

class cgc_User;
class cgc_BlogPost
{
private:
    char title[128];
    cgc_List<char *> text_blocks;
    cgc_List<cgc_File *> files;
    char post_order[32];

    int total_views;
    cgc_List<cgc_User *> likes;
    cgc_User *owner;

    cgc_BlogPost(cgc_User *user, char *_title);
public:
    static cgc_BlogPost *cgc_create_blog_post(cgc_User *user, char *_title);
    virtual ~cgc_BlogPost();
    char *cgc_get_title();
    bool cgc_add_text_block(char *text);
    bool cgc_add_file(cgc_File *file);
    void cgc_like(cgc_User *user);

    void cgc_print_post();
};

#endif

