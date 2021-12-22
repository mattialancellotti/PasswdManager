#ifndef MAIN_H
#define MAIN_H

#include <stddef.h>

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

extern char *program_db;

enum ACTION { NOPE, CRTE, CHCK, SHOW, INIT, GENE, PURG, LIST };
typedef struct {
   char *service;
   enum ACTION action;

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

#endif
