#ifndef UTILS_H_
#define UTILS_H_

int cgc_verify_hair(char *str, size_t size);
int cgc_verify_eye(char *str, size_t size);
int cgc_verify_suffix(char *str, size_t size);
int cgc_verify_state(char *str, size_t size);
int cgc_verify_education(char *str, size_t size);
int cgc_verify_month(char *str, size_t size);
int cgc_verify_day(char *str, size_t size);
int cgc_verify_year(char *str, size_t size);
int cgc_verify_height_feet(char *str, size_t size);
int cgc_verify_height_inches(char *str, size_t size);
int cgc_verify_weight(char *str, size_t size);
int cgc_verify_sex(char *str, size_t size);
int cgc_verify_yes_no(char *str, size_t size);
int cgc_verify_name(char *str, size_t size);
int cgc_verify_street(char *str, size_t size);
int cgc_verify_city(char *str, size_t size);
int cgc_verify_zip_code(char *str, size_t size);
int cgc_verify_gpa(char *str, size_t size);
int cgc_verify_email(char *str, size_t size);
int cgc_verify_phone(char *str, size_t size);
int cgc_verify_number(char *str, size_t size);
int cgc_verify_text(char *str, size_t size);

#endif /* UTILS_H_ */
