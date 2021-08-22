#ifndef MAIN_H
#define MAIN_H

#include <stddef.h>

#define RED "\x1b[31m"
#define YELLOW "\x1b[33m"
#define RESET "\x1b[0m"

#define DEFAULT_PASSWD_SIZE 8

typedef struct {
   char *service_name;
   size_t length, times;
   unsigned char_not_admitted;
} service_t;

/* 
 * Just prints out the help message.
 * It accepts an unsigned decimal that acts as a flag, if the second bit is 1
 * it will also prints the version of the program.
 */
/* TODO:
 *  - Maybe _Noreturn?
 */
void help(void);

#endif
