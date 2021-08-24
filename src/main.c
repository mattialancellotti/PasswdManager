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

/* TODO: doc */
static int confirm_identity(const char* /*program_hash*/);

#endif

/* Just the main function */
int main(int argc, char **argv)
{
#if defined(_HAVE_SODIUM)
   /*
    * Generating entropy and preparing some other things.
    * I'd recommend to take a look at https://doc.libsodium.org/usage to know
    * more about this library and its machanisms.
    */
   if (sodium_init() < 0) {
      /* Something really bad happened */
      fprintf(stderr, "Couldn't initiate the entropy.");
      return EXIT_FAILURE;
   }
#endif

#if defined(_IS_EXPERIMENTAL)
   /* TODO: Might want to declare those static and global so that I can call a
    * function on exit (atexit) to free them
    */
   char *program_hash = absolute_path(PROG_ROOT ROOT_PATH PASS_HASH);
   char *program_db   = absolute_path(PROG_ROOT ROOT_PATH PASS_DB);

   /* init */
   if (pm_init_path()) {
      fprintf(stderr, "Couldn't create the necessary directories\n");

      free(program_hash);
      free(program_db);
      return EXIT_FAILURE;
   }

#endif

   /* 
    * This struct rapresent a configuration block with the properties of a
    * password. The generated password should follow those properties.
    *
    * It has default options useful to the `handle_args` function.
    *
    * TODO: Adapt this to be both a passwd gen and a manager.
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
   if (success == -1) {
      free(program_hash);
      free(program_db);

      return EXIT_FAILURE;
   } else if (check_bit(success, (HELP|VERS))) {
      /* Checking which function needs to be called */
      if (check_bit(success, HELP))
         (void) help();
      if (check_bit(success, VERS))
         printf("Version: %.1f\n", VERSION);
      
      /* All other actions are ignored in this case */
      goto exit;
   } else if (check_bit(success, INIT)) {
#if defined(_IS_EXPERIMENTAL)
      /* TODO:
       *  - Check if the user will lose any data;
       *  - Ask the previous password to do that;
       */
      pm_init_hash(program_hash);

      goto exit;
#else
      ;
#endif
   }

#if defined(_IS_EXPERIMENTAL)
   if (confirm_identity(program_hash)) {
      free(program_hash);
      free(program_db);

      /* Tells the other that I failed :-( */
      return EXIT_FAILURE;
   }

   if (config_file.service_name != NULL) {
      pm_create_service(config_file.service_name);
      fprintf(stdout, "%s created.", config_file.service_name);
   }
#endif

   /* Temporary way of generating passwords */
   /* TODO: Solve memory leak */
   char **passwds = passwd_generator(&config_file);
   printf("Password: %s\n", passwds[0]);

exit:
#if defined(_IS_EXPERIMENTAL)
   ifdef_free(program_hash);
   ifdef_free(program_db);
#endif

   /* TODO:
    *  - return_status;
    */
   return EXIT_SUCCESS;
}

/* Just the help function */
void help()
{
   /* Pretty print */
   printf(
      "ezPass - Password Manager & Password Generator\n\n"
      "Password Manager:\n"
      "\t-s, --service     : Specifies the name of the service.\n"
      "Password Generator:\n"
      "\t-l, --length      : to specify the length of the password.\n"
      "\t-n, --not-admitted: to specify which type of character is not\n"
      "\t                    allowed [digit - u_char - l_char - sign]\n"
      "\t-t, --times       : to specify how many password you need\n"
      "Actions:\n"
      "\t    --stats       : to print the stats\n"
      "Program:\n"
      "\t    --help        : to show this message\n"
      "\t    --version     : to print the current version of the program\n"
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
      fprintf(stderr, "Wrong password\n");

      free(real_hash);
      free(passwd);
      return -1;
   }

   free(passwd);
   free(real_hash);
   return 0;
}
#endif
