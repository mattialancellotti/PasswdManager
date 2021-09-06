#ifndef ARGS_H
#define ARGS_H

#include <pass/main.h>

#define PASSWD_MIN_LENGTH 4
#define PASSWD_MAX_LENGTH 20
#define PASSWD_MAX_TIMES  10

struct primary_action {
   const char *optname;
   const char *optstring;
   unsigned has_argument;
   enum ACTION action;
};

/* 
 * Used to handle the program's arguments.
 *
 * It also returns 1 if it detects an unknown option; while it returns 0 on
 * every successfull execution.
 *
 * Also `config_file` should be defined with default options in mind. This way
 * `handle_args` can avoid predicting bad things.
 */
int handle_args(const int /*argc*/, char** /*argv*/,
                  service_t * const /*config_file*/);

int handle_args_args(const char* /*optstring*/, int /*argc*/, char** /*argv*/,
                        service_t * const /*config_file*/);


#endif
