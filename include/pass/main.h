#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>
#include <stddef.h>

#define RED "\x1b[31m"
#define YELLOW "\x1b[33m"
#define RESET "\x1b[0m"

#define DEFAULT_PASSWD_SIZE 8

typedef struct {
   char *service_name;

   struct {
      size_t length, times;
      unsigned char_not_admitted;
   };

   struct {
      bool res   : 1;
      bool init  : 1;
      bool gen   : 1;
      bool del   : 1;
      bool stat  : 1;
      unsigned   : 3;
   };
} service_t;

/* 
 * Just prints out the help message.
 * It accepts an unsigned decimal that acts as a flag, if the second bit is 1
 * it will also prints the version of the program.
 */
void help(void);

#endif
