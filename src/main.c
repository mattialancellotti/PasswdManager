#define _XOPEN_SOURCE 500

/*
 * TODO:
 *    # Urgent
 *    - Include "password encryption/decryption"
 *
 *    # Project configuration
 *    - Create a TODO file so that I can plan what it is to be done in order to
 *      get the program always to a usable state.
 *    - Configure Makefile:
 *       - install/uninstall rules
 *       - clean/doc rules
 *    
 *    # Features
 *    - Encrypts those passwords with sodium/libgcrypt.
 *    - Default action will be to print help message.
 *    - Save passwords in an XML encrypted file.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERSION 0.2

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
#endif

/* TODO: doc */
static int confirm_identity(const char* /*program_hash*/);

/* Important paths */
static const char *program_files[2] = {
   "/.local/share/ezpass/passwd",
   "/.local/share/ezpass/db/"
};

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
   char *program_hash = absolute_path(program_files[0]);
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
      goto exit;
   else if (check_bit(success, (HELP|VERS))) {
      /* Checking which function needs to be called */
      if (check_bit(success, HELP))
         (void) help();
      if (check_bit(success, VERS))
         printf("Version: %.1f\n", VERSION);
      
      /* All other actions are ignored in this case */
      return EXIT_SUCCESS;
   } else if (check_bit(success, INIT)) {
      /* TODO:
       *  - Check if the user will lose any data;
       *  - Ask the previous password to do that;
       */
#if defined(_IS_EXPERIMENTAL)
      pm_init(program_hash);
      free(program_hash);

      return EXIT_SUCCESS;
#else
      ;
#endif
   }

#if defined(_IS_EXPERIMENTAL)
   if (confirm_identity(program_hash))
      return EXIT_FAILURE;
#endif

   /* Temporary way of generating passwords */
   /* TODO: Solve memory leak */
   char **passwds = passwd_generator(&config_file);
   printf("Password: %s\n", passwds[0]);

exit:
#if defined(_IS_EXPERIMENTAL)
   ifdef_free(program_hash);
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
   printf("ezPass - Password Manager\n");
   printf("\t-l, --length      : to specify the length of the password.\n"   );
   printf("\t-n, --not-admitted: to specify which type of character is not\n");
   printf("\t                    allowed [digit - u_char - l_char - sign]\n" );
   printf("\t-t, --times       : to specify how many password you need\n"    );
   printf("\t    --stats       : to print the stats\n"  );
   printf("\t    --help        : to show this message\n");
   printf("\t    --version     : to print the current version of the program\n");
}

static int confirm_identity(const char *program_hash)
{
   char *passwd = NULL;
   char *real_hash = NULL;

   /* Asking the password */
   printf("Insert the password: ");
   passwd = ask_pass();
   printf("\n");

   /* Getting the actual hash from the file */
   real_hash = pm_hash(program_hash);
   if (real_hash == NULL) {
      fprintf(stderr, "User ezpass --init\n");

      free(passwd);
      return -1;
   }

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
