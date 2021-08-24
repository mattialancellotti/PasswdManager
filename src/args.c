#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <pass/defs.h>
#include <pass/args.h>

/* 
 * It splits the string passed to check which types of characters are not to be
 * included during the generation of the password.
 */
static int get_flags(char* /*str*/);

/* options flags */
static int use = 0, vers = 0, stat = 0, init = 0;

/*
 * Declaring arguments.
 * Since the help message or the version of the program are not really
 * password related, they have been split from the main `executing-branch` of
 * the program. Basically the struct defined below tells the `getopt_long` 
 * to assign a value to ret[x] instead of evaluating the parameter in
 * `handle_args`. This allows to write a shorter and easier code.
 *
 *    Read `man getopt.3`;
 */
/*
 * TODO:
 *    - Add `generate|g` to generate a password
 *    - Add `show|S` to show the password(s) for that service
 *    - Add `passwd|p` to specify the password for a service
 */
static const struct option options[] = {
   {"not-admitted", required_argument, 0, 'n'},
   {"service",      required_argument, 0, 's'},
   {"length",       required_argument, 0, 'l'},
   {"times",        required_argument, 0, 't'},
   {"init",         no_argument,      &init, INIT },
   {"stats",        no_argument,      &stat, STAT },
   {"help",         no_argument,      &use,  HELP },
   {"version",      no_argument,      &vers, VERS },
   {0,              0,                 0,       0 }
};

int handle_args(const int argc, char **argv, service_t * const config_file)
{
   size_t length = DEFAULT_PASSWD_SIZE, times = 1;
   int option_index = 0, c, success = 0;

   /* Parsing arguments using getopt_long(..), check man getopt.3 */
   while (1) {
      c = getopt_long(argc, argv, "l:n:t:s:", options, &option_index);
      if (c == -1)
         break;

      switch (c) {
      case 0:
         /* Found but should not handle it */
         break;
      case '?':
         /* Unsuccessfull matching */
         return -1;
      case 's':
         config_file->service_name = optarg;
         break;
      case 'l':
         /* Sets the length of the password */
         if ((length = (size_t)atoi(optarg)) && length >= PASSWD_MIN_LENGTH 
                                             && length <= PASSWD_MAX_LENGTH)
            /* If not then leave the default option */
            config_file->length = length;

         break;
      case 'n':
         /* Uses `get_flags(..)` to get an unsigned flag of banned characters */
	 config_file->char_not_admitted = get_flags(optarg);
         if (config_file->char_not_admitted == 15) {
            fprintf(stderr, "Fatal: can't accept all types as argument\n");
            return -1;
         }

         break;
      case 't':
         /* Sets the length of the password */
         if ((times = atoi(optarg)) && times <= PASSWD_MAX_TIMES)
            /* If not then leave the default option */
            config_file->times = times;
         break;
      default:
         /* Exits unsuccessfully */
         return -1;
      }
   }

   /* Returns successfully */
   set_bit(success, init);
   set_bit(success, stat);

   /* These two have the priority over the previous ones */
   if (use || vers)
      set_bit(success, ((success & ~success) | (use | vers)));

   return success;
}

#define MAX_NOT_ADMITTED_STR_NUM 4
static int get_flags(char *str)
{
   size_t i=0, flags=0;  // 0 ->digit, 0->l_char, 0->u_char, 0->sign

   char *token = NULL;

   /* 
    * The strtok_r function creates a token every times it loops through the
    * string.
    */
   while ((token = strtok_r(str, ",", &str)) && i < MAX_NOT_ADMITTED_STR_NUM) {
      if (!strcmp(token, "digit"))
	 set_bit(flags, 8);
      else if (!strcmp(token, "l_char"))
	 set_bit(flags, 4);
      else if (!strcmp(token, "u_char"))
	 set_bit(flags, 2);
      else if (!strcmp(token, "sign"))
	 set_bit(flags, 1);
      else
         /* 
          * Returning 0 so we don't have to worry about checking stuff, it just
          * acts as nothing happened.
          */
         return 0;

      /* Counting tokens */
      i++;
   }

   return flags;
}
