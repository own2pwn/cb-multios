#ifndef CMDB_BACKEND_H_
#define CMDB_BACKEND_H_

typedef struct {
    char *name;
    char *desc;
    short year;
    char score;
    char *type;
    char *mpaa;
    char is_checked_out;
}  cgc_cmdb_entry_t;

typedef enum {ALL, OWNED, RENTED} cgc_filter_e;

char *cgc_get_genre(int id);
char *cgc_get_rating(int id);
size_t cgc_get_list_length();
size_t cgc_get_num_rented();
int cgc_add_movie(char *name, char *desc, short year, char score, char *type, char *mpaa);
int cgc_add_entry(cgc_cmdb_entry_t *entry);
int cgc_delete_entry(int id);
int cgc_rent_entry(int id);
int cgc_return_entry(int id);
cgc_cmdb_entry_t *cgc_find_entry(char *name);
cgc_cmdb_entry_t *cgc_get_entry(int id);
void cgc_print_entry(cgc_cmdb_entry_t *entry);
void cgc_print_movies(cgc_filter_e filter);
void cgc_print_genres();
void cgc_print_ratings();
void cgc_dag(char *new_genre);
void cgc_dar(char *new_rating);

#endif
