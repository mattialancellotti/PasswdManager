#ifndef STATS_H
#define STATS_H

#include <stddef.h>

/* This is where the password is stored along with some information about it */
typedef struct {
  char * restrict passwd;
  
  int consecutive_u_char, consecutive_l_char, consecutive_digit, consecutive_sign;
  int repeated_u_char, repeated_l_char, repeated_digit, repeated_sign;
  int number_digit, number_l_char, number_u_char, number_sign;
} passwd_t;

/* Types of characters */
enum TYPE { SIGN = 0, U_CHAR = 2, L_CHAR = 4, DIGIT = 8,  NONE = 0 };

/*
 * This one accepts the password and its length to execute some controls on it.
 * This is useful if you want to have some information about your password
 * like how many numbers are in it and things like that.
 */
void check_passwd(passwd_t ** /*passwd_creted*/, const size_t length);

/*
 * This function prints out to the user the information gathered through the
 * `check_passwd` function.
 */
void print(const passwd_t * /*passwd_info*/);

/* Searches a char in the given string and returns its position */
size_t search(const char *, const size_t, const char);

#endif
