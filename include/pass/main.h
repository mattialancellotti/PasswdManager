#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>
#include <stddef.h>

#define RED "\x1b[31m"
#define YELLOW "\x1b[33m"
#define RESET "\x1b[0m"

#define NO_HASH "You should initialize the new database before using the " \
                "pogram.\nTo do so type 'ezpass --init' and follow the "   \
                "instructions."

#define PASSWDS "There are still saved passwords in your database and " \
                "by re-initiating it you will lose all of them.\n"      \
                "Are you sure you want to continue? [y/N] "

#define PURGE_P "Are you sure you want to delete all your passwords? [y/N] "

#define NO_INIT "Something went wrong during the initializaiton but all your" \
                " passwords are still there."

#define DEFAULT_PASSWD_SIZE 8

typedef struct {
   char *service;

   struct {
      size_t length, times;
      unsigned char_not_admitted;
   };
} service_t;

/* 
 * Just prints out the help message.
 * It accepts an unsigned decimal that acts as a flag, if the second bit is 1
 * it will also prints the version of the program.
 */
void help(void);

/* TODO: Put this into its own file with some other user input functions */
char ask_confirmation(const char* /*msg*/);

#endif
