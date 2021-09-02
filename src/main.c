#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define VERSION 0.3

#if defined(_HAVE_SODIUM)
#  include <sodium.h>
#endif

#if defined(_HAVE_DEBUG)
#  include <assert.h>
#endif

#include <pass/defs.h>
#include <pass/args.h>
#include <pass/gen.h>

#if defined(_IS_EXPERIMENTAL)
#  include <pass/term.h>
#  include <pass/os.h>
#  include <pass/crypto.h>
#  include <pass/pwman.h>
#  include <pass/stats.h>

/* The program's main files */
static char *program_root;
static char *program_hash;
static char *program_db;

/* TODO: doc */
static int init_prog_env(void);
static int confirm_identity(const char* /*program_hash*/);
static void free_files(void);

#endif

int main(int argc, char **argv)
{
   /* If there are no arguments prints the help message and be done with it. */
   if (argc == 1) {
      (void) help();
      return EXIT_SUCCESS;
   }

   /*  TODO: update this
    * This struct rapresent a configuration block with the properties of a
    * password. The generated password should follow those properties.
    *
    * It has default options useful to the `handle_args` function.
    */
   service_t config_file = { 
      .service = NULL,
      .action = EMPTY,
      .length = DEFAULT_PASSWD_SIZE,
      .times  = 1,
      .char_not_admitted = 0,
   };

   /* TODO:
    *  - Create a list of errors (take a look at the OSH project);
    */
   /* Handling arguments */
   int success = handle_args(argc, argv, &config_file);
   if (success == -1)
      return EXIT_FAILURE;
   else if (check_bit(success, (HELP|VERS))) {
      /* Checking which function needs to be called */
      if (check_bit(success, HELP))
         (void) help();
      if (check_bit(success, VERS))
         printf("Version: %.1f\n", VERSION);
      
      /* All other actions are ignored in this case */
      return EXIT_SUCCESS;
   }

#if defined(_HAVE_SODIUM)
   /*
    * Generating entropy and preparing some other things.
    * I'd recommend to take a look at https://doc.libsodium.org/usage to know
    * more about this library and its machanisms.
    */
   if (sodium_init() < 0) {
      /* Something really bad happened */
      fprintf(stderr, "Couldn't initiate the entropy.\n");
      return EXIT_FAILURE;
   }
#endif

#if defined(_IS_EXPERIMENTAL)
   exit_if(init_prog_env(), EXIT_FAILURE);

   /*
    * Long story short if the user is initializing a new hash with
    * a non-empty database he'll be asked whether to continue or not.
    */
   if (strict_check(success, INIT)) {
      if (!is_empty(program_db)) {
         printf("Initializing the new database of passwords.\n");

         /* Checking if the initialization worked */
         warn_user((pm_init_hash(program_hash) == -1),
                     "Couldn't complete the initialization.\n", EXIT_FAILURE);
         
         /* Telling the user everything worked out */
         printf("Initializationg completed.\n");
         return EXIT_SUCCESS;
      }

      /*
       * If the user is calling 'ezpass --init' on a non-empty ask him if he
       * wants to delete all the saved passwords by re-initializing the
       * database.
       */
      if (confirm_identity(program_hash)) return EXIT_FAILURE;
      if (ask_confirmation(PASSWDS) == 'y') {
         /* Re-initializing the hash and deleting the old passwords */
         warn_user((pm_init_hash(program_hash) == -1), NO_INIT, EXIT_FAILURE);
         exit_if((pm_purge_db(program_db) == -1), EXIT_FAILURE);

         /* If everything went as planned just notify the user and exit */
         printf("All your passwords are gone and your database is now clear.\n");
      }

      /* This option intentionally ignores every other option */
      return EXIT_SUCCESS;
   }

   /* Checking if the user just wants to generate a new password */
   if (strict_check(success, GENE) && config_file.service == NULL) {
      char *passwd = create_passwd(config_file.length,
                                       config_file.char_not_admitted);
      printf("Password: %s\n", passwd);

      /* Printing stats about the password */
      if (check_bit(success, STAT))
         (void) passwd_stats(passwd);

      free(passwd);
      return EXIT_SUCCESS;
   }

   /* If there is no hash to verify notify the user and eixt */
   warn_user((!exists(program_hash)), NO_HASH, EXIT_FAILURE);

   /*
    * All the action handling except for `--init` should stay after the
    * confirmation block.
    */
   if (confirm_identity(program_hash))
      /* Tells the others that I failed :-( */
      return EXIT_FAILURE;

   int err = 0;
   switch (config_file.action) {
   case CREAT:
      /* Creating a new service called `config_file.service` */
      err = pm_create_service(config_file.service);
      exit_if((err == -1), EXIT_FAILURE);

      /* Updating the password */
      if (strict_check(success, GENE)) {
         char *passwd = create_passwd(config_file.length,
                                       config_file.char_not_admitted);
         err = pm_update_service(config_file.service, passwd);
         exit_if((err == -1), EXIT_FAILURE);
      } else {
      }

      /* Informing the user about the success */
      printf("Service '%s' created.\n", config_file.service);
      break;
   case PURG:
      /* Checking if an argument has been supplied to purge */
      if (config_file.service != NULL) {
         /* Purge the specified service */
         err = pm_delete_service(config_file.service);
         exit_if((err == -1), EXIT_FAILURE);
         printf("Service '%s' deleted.\n", config_file.service);

         break;
      }

      /* This action will only purge the passwords saved in the database */
      if (ask_confirmation(PURGE_P) == 'y') {
         exit_if((pm_purge_db(program_db) == -1), EXIT_FAILURE);
      } else
         printf("No password was harmed in the process. :)\n");

      break;
   case SHOW:
      err = pm_read_service(config_file.service);
      exit_if((err == -1), EXIT_FAILURE);
      break;
   case LIST:
   case EMPTY:
   default:
      (void) help();
      break;
   }

#endif

   return EXIT_SUCCESS;
}

/* Just the help function */
void help()
{
   /* Pretty print */
   printf(
      "ezPass - Password Manager & Password Generator\n\n"
      "  Password Manager:\n"
      "\t-s, --service     : Specifies the name of the service.\n"
      "\t    --init        : Used to initiate a new databased of passwords.\n"
      "  Password Generator:\n"
      "\t-l, --length      : To specify the length of the password.\n"
      "\t-n, --not-admitted: To specify which type of character is not\n"
      "\t                    allowed [digit - u_char - l_char - sign].\n"
      "\t-t, --times       : To specify how many password you need.\n"
      "  Actions:\n"
      "\t    --stats       : Analyzes the password.\n"
      "\t    --purge       : Clears the database from passwords.\n"
      "\t    --generate    : Generates a password based on the specific arguments.\n"
      "  Program:\n"
      "\t    --help        : Shows this message.\n"
      "\t    --version     : Prints the current version of the program.\n"
   );
}

#if defined(_IS_EXPERIMENTAL)
static int init_prog_env(void)
{
   program_root = absolute_path(PROG_ROOT ROOT_PATH);
   program_hash = absolute_path(PROG_ROOT ROOT_PATH PASS_HASH);
   program_db   = absolute_path(PROG_ROOT ROOT_PATH PASS_DB);
   prog_err(atexit(free_files), "Couldn't set exit function.", _Exit(EXIT_FAILURE));

   /* init */
   int err_path = pm_init_path();
   warn_user(err_path, "Couldn't create the program's db.", EXIT_FAILURE);

   return 0;
}

static int confirm_identity(const char *program_hash)
{
   /* Getting the actual hash from the file */
   char *real_hash = pm_hash(program_hash);
   if (real_hash == NULL)
      return -1;

   /* Asking the password */
   printf("Unlock the password manager: ");
   char *passwd = ask_pass();
   printf("\n");

   /* Checking the password */
   if (hash_check(real_hash, passwd) == -1) {
      fprintf(stderr, "Wrong password foo.\n");

      free(real_hash);
      free(passwd);
      return -1;
   }

   free(passwd);
   free(real_hash);
   return 0;
}

static void free_files(void)
{
   printf("Bye Bye :D\n");

   ifdef_free(program_hash);
   ifdef_free(program_db);
   ifdef_free(program_root);
}

#define case_ins(var, ch) ((var == ch) || (var == (ch+32)))
char ask_confirmation(const char *msg)
{
   const char *default_msg = "Are you sure? (y/N) >>";
   const char short_yes = 'y', short_no = 'n';
   char *answer = NULL;
   char ret = '\0';

   do {
      printf("%s", ( msg == NULL ? default_msg : msg));
      answer = users_input();

      /* If the users is trying the be smart, outsmart him */
      if (case_ins(answer[0], 'Y') || case_ins(answer[0], 'N'))
         ret = tolower(answer[0]);
      else
         fprintf(stderr, "'%s', what's that?", answer);

      free(answer);
   } while (ret == '\0');

   /* Returing successfully */
   return ret;
}
#endif
