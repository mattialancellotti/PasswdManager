#define _XOPEN_SOURCE 500

/*
 * TODO:
 *    # Urgent
 *    - Include "password encryption/decryption"
 *
 *    # Project configuration
 *    - Create a TODO file so that I can plan what it is to be done in order to
 *      get the program always to a usable state.
 *    
 *    # Features
 *    - Encrypts those passwords with sodium/libgcrypt.
 *    - Default action will be to print help message.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

/* The program's main files */
static char *program_hash;
static char *program_db;

/* TODO: doc */
static int confirm_identity(const char* /*program_hash*/);
static void free_files(void);
static char ask_confirmation(const char* /*msg*/);

#endif

int main(int argc, char **argv)
{
   /* If there are no arguments prints the help message and be done with it. */
   if (argc == 1) {
      (void) help();
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
   program_hash = absolute_path(PROG_ROOT ROOT_PATH PASS_HASH);
   program_db   = absolute_path(PROG_ROOT ROOT_PATH PASS_DB);
   prog_err(atexit(free_files), "Couln't set exit function.", _Exit(EXIT_FAILURE));

   /* init */
   int err_path = pm_init_path();
   prog_err(err_path, "Couldn't create the program's db.", return EXIT_FAILURE);

#endif

   /* 
    * This struct rapresent a configuration block with the properties of a
    * password. The generated password should follow those properties.
    *
    * It has default options useful to the `handle_args` function.
    */
   service_t config_file = { 
      .service_name = NULL,
      .length = DEFAULT_PASSWD_SIZE,
      .times  = 1,
      .char_not_admitted = 0,
   };

   /* TODO:
    *  - Create a list of errors (take a look at the OSH project);
    *  - This piece of code produces memory leak if _IS_EXPERIMENTAL is enabled.
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
   
#if defined(_IS_EXPERIMENTAL)
   if (config_file.init) {
      /* 
       * There is no reset mechanism here other than forcing the user into
       * calling the program a second time with the --reset argument. This is
       * mainly due to lazyness and security/awareness seemed to be a good
       * excuse.
       */
      if (!is_empty(program_db))
         pm_init_hash(program_hash);
      else
         fprintf(stderr, "Use the option --reset if you really "
                         "want to delete all your saved passwords.\n");

      return EXIT_SUCCESS;
   }

   /*
    * All the action handling except for `--init` should stay after the
    * confirmation block.
    */
   if (confirm_identity(program_hash))
      /* Tells the others that I failed :-( */
      return EXIT_FAILURE;

   /* Handling resetting */
   if (config_file.res) {
      /* TODO: implement a rmpath that does rm recursively */
   }

   /* All the other actions [--stat, --generate ] are 'service' specific */
   if (config_file.service_name != NULL) {
      /* TODO: check if pm_create_service failed */
      pm_create_service(config_file.service_name);
      printf("Service %s created.", config_file.service_name);

      /* TODO: Implement stats, generate e show */
   } else if (config_file.gen) {
      /* Temporary way of generating passwords */
      /* TODO: Solve memory leak */
      char **passwds = passwd_generator(&config_file);
      printf("Password: %s\n", passwds[0]);
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
      "  Password Generator:\n"
      "\t-l, --length      : To specify the length of the password.\n"
      "\t-n, --not-admitted: To specify which type of character is not\n"
      "\t                    allowed [digit - u_char - l_char - sign].\n"
      "\t-t, --times       : To specify how many password you need.\n"
      "  Actions:\n"
      "\t    --stats       : Analyzes the password.\n"
      "\t    --reset       : Clears the database from passwords.\n"
      "\t    --init        : Used to initiate a new databased of passwords.\n"
      "\t    --delete      : Can be used to delete a password.\n"
      "\t    --generate    : Generates a password based on the specific arguments.\n"
      "  Program:\n"
      "\t    --help        : Shows this message.\n"
      "\t    --version     : Prints the current version of the program.\n"
   );
}

#if defined(_IS_EXPERIMENTAL)
static int confirm_identity(const char *program_hash)
{
   /* Getting the actual hash from the file */
   char *real_hash = pm_hash(program_hash);
   if (real_hash == NULL)
      return -1;

   /* Asking the password */
   printf("Insert the password: ");
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
}

static char ask_confirmation(const char *msg)
{
   const char *default_msg = "Are you sure? (y/N) >>";
   char *answer = NULL;

   printf("%s", ( msg == NULL ? default_msg : msg));
   answer = users_input();

   /* If the users is trying the be smart, outsmart him */
   if (answer == NULL || answer[0] != 'y' || answer[0] != 'Y'
                      || answer[0] != 'n' || answer[0] != 'N')
      return 'n';

   return 'y';
}
#endif
