#ifndef ARGS_H
#define ARGS_H

#include "main.h" 

//ARGS_ERROR
#define UNKNOWN_ARG       1
#define PARAMETER_MISSING 2
#define PARAMETER_ERROR   4
#define COMING_SOON_ARG   8

enum TYPE { DIGIT, U_CHAR, L_CHAR, SIGN, NONE };

passwd_mod_t *handle_args(const char **, const int);
void print_args_error(const size_t);
unsigned get_flags(const char *);

#endif
