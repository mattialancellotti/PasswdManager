#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>
#include <stdio.h>

#define RED "\x1b[31m"
#define YELLOW "\x1b[33m"
#define RESET "\x1b[0m"

#define DEFAULT_PASSWD_SIZE 8

/* This is where the password is stored along with some information about it */
/*
 * TODO
 *  This needs to be deleted/updated since it's not usefull anymore
 */
typedef struct {
  char * restrict passwd;
  
  int consecutive_u_char, consecutive_l_char, consecutive_digit, consecutive_sign;
  int repeated_u_char, repeated_l_char, repeated_digit, repeated_sign;
  int number_digit, number_l_char, number_u_char, number_sign;
} passwd_t;

/*
 * TODO
 *  This struct needs to be updated with further information about the password
 *  and the profile structure like categories of characters or the default
 *  length for that password, the name of the service.
 */
typedef struct {
  char *name, desc;
  size_t length;
  int nchars;
} passwd_profile_t;

/* DEPRECATED */
typedef struct {
  char *saving_point;
  size_t length, times;
  int char_not_admitted,
      saving_functionality;
} passwd_conf_t;

/* This struct represent settings.yml */
typedef struct {
  char *passwds_file;
  size_t max_l, min_l;
  unsigned accept_same_password,
           colors, profiles;
} config_t;

/* Types of characters */
enum TYPE { SIGN = 0, U_CHAR = 2, L_CHAR = 4, DIGIT = 8,  NONE = 0 };

/*
 * This function creates a password of length `length`. Also  it accepts a
 * `flags` that indicates which of { 'u_char', 'l_char' ... } are not accepted
 */
char *create_passwd(const size_t /*length*/, const int /*flags*/);

/*
 * This one accepts the password and its length to execute some controls on it.
 * This is useful if you want to have some information about your password
 * like how many numbers are in it and things like that.
 */
void check_passwd(passwd_t ** const /*passwd_created*/, const size_t /*length*/);

/*
 * This function prints out to the user the information gathered through the
 * `check_passwd` function.
 */
void print(const passwd_t * const /*passwd_info*/);

#endif
