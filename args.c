#include <string.h>

#include "args.h"

/* 
 * It splits the string passed to check which types of characters are not to be
 * included during the generation of the password.
 */
static int get_flags(char* /*str*/);

#define SET_BIT(bitarr, bitpos) ((bitarr) |= (bitpos))
int handle_args(const int argc, char **argv,
      passwd_conf_t * const restrict config_file)
{
   size_t length = DEFAULT_PASSWD_SIZE, times = 1;
   int option_index = 0, c, success = 0;

   /* options flags */
   int fh = 0, fp = 0, fv = 0;

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
    * TODO
    *    - Add a `list-profiles` to list profiles (default action)
    *    - Add a `del-profile` to delete the chosen profile (Not sure)
    *    - Add a `profile` to select a profile to generate that password
    *    - Add a 'config' to load a different config file
    *    - Remove `save` because profiles should do it
    *    - Add `encryption` to decide which algorithm is to be used (Not sure)
    *    - Rename `print` to `stats`
    *    - Add `temp` to generate a temporary password for One Time Use
    *      (means that I need to implement On Time Passwords)
    */
   const struct option options[] = {
      {"not-admitted", required_argument, 0, 'n'},
      {"length",       required_argument, 0, 'l'},
      {"save",         required_argument, 0, 's'},
      {"times",        required_argument, 0, 't'},
      {"stats",        no_argument,      &fp, 3 },
      {"help",         no_argument,      &fh, 1 },
      {"version",      no_argument,      &fv, 2 },
      {0,              0,                 0,  0 }
   };

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
         return 1;
      case 's':
         config_file->saving_functionality = ( optarg[0] == 'y' ? 1 : 0 );
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
   SET_BIT(success, fp);
   SET_BIT(success, fh);
   SET_BIT(success, fv);

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
	 SET_BIT(flags, 8);
      else if (!strcmp(token, "l_char"))
	 SET_BIT(flags, 4);
      else if (!strcmp(token, "u_char"))
	 SET_BIT(flags, 2);
      else if (!strcmp(token, "sign"))
	 SET_BIT(flags, 1);
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
