#ifndef MAIN_H
#define MAIN_H

#include <stddef.h>

#define RED "\x1b[31m"
#define YELLOW "\x1b[33m"
#define RESET "\x1b[0m"

#define DEFAULT_PASSWD_SIZE 8

/* TODO:
 *  - Rename to service_t (just more sense);
 */
typedef struct {
   size_t length, times;
   unsigned char_not_admitted;
} passwd_conf_t;

/* This struct represent settings.yml */
/* TODO:
 *  - Delete this thing;
 */
typedef struct {
   char *passwds_file;
   unsigned max_l, min_l;
   unsigned accept_same_password,
            colors, profiles;
} config_t;

/*
 * This function creates a password of length `length`. Also  it accepts a
 * `flags` that indicates which of { 'u_char', 'l_char' ... } are not accepted
 */
char *create_passwd(const size_t /*length*/, const int /*flags*/);

/* 
 * Just prints out the help message.
 * It accepts an unsigned decimal that acts as a flag, if the second bit is 1
 * it will also prints the version of the program.
 */
/* TODO:
 *  - Maybe _Noreturn?
 */
void help(void);

char *load_hash(void);

#endif
