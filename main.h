#ifndef MAIN_H
#define MAIN_H

#include<stdlib.h>
#include<stdio.h>

#define RED "\x1b[31m"
#define YELLOW "\x1b[33m"
#define RESET "\x1b[0m"

#define VERSION 2.8
#define MAJOR_RELEASE_DATE "2019-11-04"
#define MINOR_RELEASE_DATE  "2019-11-18"
#define DEFAULT_PASSWD_SIZE 8

typedef struct {
	char * restrict passwd;
	
	int consecutive_u_char, consecutive_l_char, consecutive_digit, consecutive_sign;
	int repeated_u_char, repeated_l_char, repeated_digit, repeated_sign;
	int number_digit, number_l_char, number_u_char, number_sign;
} passwd_t;

typedef struct {
	passwd_t ** passwd_list;
	char *module_name;

	size_t length, times;
	int char_not_admitted_flags, args_error_flags, args_flags;
	//args_error_flags saves which errors occurred
	//args_flags choices made by the user
} passwd_mod_t;


char *create_passwd(const size_t, const int);
void check_passwd(passwd_t ** const, const size_t);
void print(const passwd_t * const);
#endif
