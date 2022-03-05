#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define VERSION 0.3

#if defined(_HAVE_SODIUM)
#  include <sodium.h>
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
#  include <pass/services.h>

/* The program's main files */
static char *program_root;
static char *program_hash;
char *program_db = NULL;

/* TODO: doc */
static int init_prog_env(void);
static int confirm_identity(const char* /*program_hash*/);
static void free_files(void);

#endif

int main(int argc, char **argv)
{
   init_prog_env();
   char *password = malloc(5);
   unsigned long long length = 0;

   password = strcpy(password, "1234");
   unsigned char *encrypted_passwd = encrypt_password(password, password, &length);
   int c = append_service("test", (const char*)encrypted_passwd);

   //unsigned char *decrypted_passwd = decrypt_password(encrypted_passwd, password);

   //printf("Cleartext: %s\n", decrypted_passwd);
   return 0;
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
      .action = NOPE,
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

   if (config_file.action == GENE) {
      char *passwd = create_passwd(config_file.length,
                                    config_file.char_not_admitted);
      printf("Generated password: %s\n", passwd);
      free(passwd);

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

   /* Checks an hash has already been saved before asking for authentication */
   if (exists(program_hash) && confirm_identity(program_hash))
      return EXIT_FAILURE;

   int err = 0;
   switch (config_file.action) {
   case CRTE:
      /* Creating the service */
      err = create_service(config_file.service);
      /* TODO: write to the file */
      break;
   case CHCK:
      break;
   case SHOW:
      err = expose_service(config_file.service);
      break;
   case INIT:
      if (is_empty(program_db)) {
         if (ask_confirmation(PASSWDS) == 'n') {
            printf("No database initialized\n");
            break;
         }
      }

      /* TODO What if the program cannot delete the db? */
      err = pm_purge_db(program_db);

      err = pm_init_hash(program_hash);
      if (err == -1)
         fprintf(stderr, "Passwords must match\n");

      break;
   case PURG:
      if (config_file.service == NULL) {
         err = pm_purge_db(program_db);
         break;
      }

      err = delete_service(config_file.service);
      break;
   case LIST:
   default:
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
#endif
