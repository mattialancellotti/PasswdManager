#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <limits.h>
#include <assert.h>

#include <pass/defs.h>
#include <pass/args.h>

/* 
 * It splits the string passed to check which types of characters are not to be
 * included during the generation of the password.
 */
static int get_flags(char* /*str*/);

/* options flags */
static int vers = 0, use  = 0;
static int ask  = 0, verb = 0;
static int forc = 0, colr = 0;

int behaviour = 0;
int options_args = 0;

/*
 * Declaring arguments.
 * Since the help message or the version of the program are not really
 * password related, they have been split from the main `executing-branch` of
 * the program. Basically the struct defined below tells the `getopt_long` 
 * to assign a value to a flag instead of evaluating the parameter in
 * `handle_args`. This allows to write a shorter and easier code.
 *
 *    Read `man getopt.3`;
 */
static const struct option args_options[] = {
   {"version", no_argument, &vers, VERS},
   {"verbose", no_argument, &verb, VERB},
   {"colors",  no_argument, &colr, COLR},
   {"force",   no_argument, &forc, FORC},
   {"help",    no_argument, &use,  HELP},
   {"ask",     no_argument, &ask,  ASK_},
   {"not-admitted", required_argument, 0, 'n'},
   {"length",       required_argument, 0, 'l'},
   {"generate",     no_argument, 0, 'g'},
   {"fancy",        no_argument, 0, 'f'},
   {"info",         no_argument, 0, 'i'},
   {"pacman",       no_argument, 0, 'p'},
   {"check",        no_argument, 0, 'c'},
   {0,       0,               0,     0 }
};

static const struct primary_action actions[] = {
   {"gen",   "l:n:c", no_argument,       GENE},
   {"check",  "fpb:", optional_argument, CHCK},
   {"create",    "g", required_argument, CRTE},
   {"show",     "fi", required_argument, SHOW},
   {"purge",    NULL, optional_argument, PURG},
   {"init",   "l:n:", no_argument,       INIT},
   {"list",     NULL,       no_argument, LIST}
};

#define PRIMARY_INDEX (1)
int handle_args(const int argc, char **argv, service_t * const config_file)
{
   size_t length = DEFAULT_PASSWD_SIZE, times = 1;
   int option_index = 0, c, success = 0;

   size_t pa_length = sizeof(actions)/sizeof(struct primary_action);
   int offset = 0, action = -1;
   for (size_t i = 0; i < pa_length; i++) {
      if (!strcmp(argv[PRIMARY_INDEX], "--help")) {
         set_bit(success, HELP);
         offset = 1;
         break;
      } else if (!strcmp(argv[PRIMARY_INDEX], "--version")) {
         set_bit(success, VERS);
         offset = 1;
         break;
      } else if (!strcmp(argv[PRIMARY_INDEX], actions[i].optname)) {
         action = i;
         config_file->action = actions[i].action;

         /* Checking what kind of parameter it needs */
         switch (actions[i].has_argument) {
         case required_argument:
            if (PRIMARY_INDEX == argc-1) {
               fprintf(stderr, "The option '%s' requires an argument.\n", 
                               argv[PRIMARY_INDEX]);
               return -1;
            }
         case optional_argument:
            if (argc-1 > PRIMARY_INDEX) {
               printf("Program: %s, Primary action '%s' on '%s'\n", argv[0], argv[1], argv[2]);
               config_file->service = argv[2];
               offset = 2;

               break;
            }
         case no_argument:
            printf("Program: %s, Primary action '%s'\n", argv[0], argv[1]);
            offset = 1;
            break;
         default:
            break;
         }

         /* Exiting after success */
         break;
      }
   }

   if (!offset) {
      fprintf(stderr, "Option '%s' unknown.\n", argv[1]);
      fprintf(stderr, "Try using --help next time.\n");
      return -1;
   }

#if defined(_HAVE_DEBUG)
   printf("Pointer: %d - %s\n", argc-offset, argv[offset]);
#endif

   /* Parsing the rest of the arguments */
   int args_success = 0;
   if (argc-offset != 1) {
      const char *optstr = (action == -1 ? "" : actions[action].optstring);
      args_success = handle_args_args(optstr, argc-offset, 
                                        argv+offset, config_file);
   }

   warn_user((args_success == -1), "Try with --help next time.", -1);
   success |= args_success;

#if defined(_HAVE_DEBUG)
   printf("Flags: %d\n", success);
   printf("Behaviour: %d\n", behaviour);
   printf("Options: %d\n", options_args);
#endif

   return success;
}

int handle_args_args(const char *optstring, int argc, char **argv,
                        service_t * const config_file)
{
   int success = 0, longindex = 0;
   int c = 0, flags;
   long length = 0;

   while (1) {
      c = getopt_long(argc, argv, optstring,
                        args_options, &longindex);
      if (c == -1)
         break;

      switch (c) {
         case '?':
            /* Unknown option, missing argument */
            return -1;
         case ':':
            /* Found but has an optional argument */
            break;
         case 'g': set_bit(options_args, GNRT); break;
         case 'p': set_bit(options_args, PCMN); break;
         case 'c': break;
         case 'f': set_bit(options_args, FNCY); break;
         case 'i': set_bit(options_args, INFO); break;
         case 'l':
            length = strtol(optarg, NULL, 10);
            if (length != LONG_MIN && length != LONG_MAX)
               config_file->length = (size_t)length;

            break;
         case 'n':
            flags = get_flags(optarg);
            if (flags < 16) {
               config_file->char_not_admitted = flags;
               break;
            }

            fprintf(stderr, "Nope.\n");
            break;
         default:
            break;
      }
   }

   /* Checking flags */
   set_bit(behaviour, (verb|colr|forc|ask));
   set_bit(success, (use|vers));

#if defined(_HAVE_DEBUG)
   printf("Success: %d\n", success);
#endif
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
