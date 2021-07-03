#ifndef ARGS_H
#define ARGS_H

#include <getopt.h>

#include "main.h"

#define PASSWD_MIN_LENGTH 4
#define PASSWD_MAX_LENGTH 20
#define PASSWD_MAX_TIMES  10

/* 
 * Used to handle the program's arguments.
 *
 * Don't mind the restricted pointer on `config_file`. Just a way to avoid that
 * multiple functions modify it. I don't think it's going to happen since the
 * program it's not even mutlithreaded.
 *
 * It also returns 1 if it detects an unknown option; while it returns 0 on
 * every successfull execution.
 *
 * Also `config_file` should be defined with default options in mind. This way
 * `handle_args` can avoid predicting bad things.
 */
int handle_args(const int argc, char** /*argv*/, 
                  passwd_conf_t * const restrict /*config_file*/);

#endif
