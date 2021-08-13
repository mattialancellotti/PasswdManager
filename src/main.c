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
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

#if defined(__libsodium__)
#  include <sodium.h>
#endif

#if defined(__debug__)
#  include <assert.h>
#endif

#include <pass/defs.h>
#include <pass/storage.h>
#include <pass/args.h>
#if defined(__experimental__)
#  include <pass/term.h>
#  include <pass/os.h>
#  include <pass/crypto.h>
#endif

/* TODO doc */
static void mem_dealloc_passwd_t(passwd_conf_t** /*passwd_s*/);
static passwd_conf_t* mem_alloc_passwd_t(void);

/* Just the main function */
int main(int argc, char **argv)
{
#if defined(__libsodium__)
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

#if defined(__experimental__)
   /*
    * TODO
    *    Ask for a password key to decrpyt files.
    *    This is to be done here so that there won't be any kind of useless load
    *    if the user enters the wrong password.
    */
   char *passwd = ask_pass();
   printf("Password: %s\n", passwd);
   
   char *home = users_path();
   printf("%s\n", home);
   free(home);


   char *out = hash_password(passwd);
   printf("%s\n", out);

   /* Checking the password */
   if (crypto_pwhash_str_verify(out, "ciao", strlen("ciao")) == -1)
      perror("Wrong password");

   free(out);
   free(passwd);
   return 0;

#endif
   /* 
    * This struct rapresent a configuration block with the properties of a
    * password. The generated password should follow those properties.
    *
    * It has default options useful to the `handle_args` function.
    */
   passwd_conf_t config_file = { 
      .length = DEFAULT_PASSWD_SIZE,
      /* Temporarily disabled */
      .times  = 1,
      .char_not_admitted = 0,
   };


   /* Checking for errors and then tries to exit the right way */
   int success;
   if ((success = handle_args(argc, argv, &config_file)) == -1)
      return EXIT_FAILURE;

   /* Checking if the user wants to see the help message and the version */
#define VERSION 0.2
   if (success & 2)
      printf("Version: %.1f\n", VERSION);

   if (success & 1) {
      /* This piece of code is ugly as fuck */
      (void) help();
      return EXIT_SUCCESS;
   }

   /* Allocating the array of passwords */
   char *passwords[config_file.times];

   /* Creates the passwords, saves them and execute the check_passwd on them */
   for (size_t i=0; i<config_file.times; i++) {
      /* Creating the password */
      passwords[i] = create_passwd(config_file.length, config_file.char_not_admitted);
      printf("Password: %s%s%s\n", YELLOW, passwords[i], RESET);

      free(passwords[i]);
      /* Wait unless this is the last password */
      if (config_file.times != i+1)
         /*
          * This is needed to create new seeds, without the sleep function all
          * the passwords generated are going to be the same.
          */
	 sleep(1);
   }

   return EXIT_SUCCESS;
}

/* TODO:
 *  - Move this to its own file
 */
/* Really basic way of creating a password */
char *create_passwd(const size_t length, const int flags)
{
   char *passwd = malloc(sizeof(char)*length+1);
   size_t i=0;
   srand(time(NULL));
   
   while(i<length) {
      passwd[i] = rand()%94+33;
      /* This if checks if the picked character is accepted or not */
      if ((isdigit(passwd[i]) && flags&8) || (islower(passwd[i]) && flags&4)
					  || (isupper(passwd[i]) && flags&2)
					  || (ispunct(passwd[i]) && flags&1))
         continue;
      
      i++;
   }

  passwd[i] = '\0';

  return passwd;
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

/* Safely deallocates a password */
static void mem_dealloc_passwd_t(passwd_conf_t ** passwd_s)
{
   /* Some controls to avoid segmatation faults */
   if (*passwd_s) {
      free(*passwd_s);
      *passwd_s = NULL;
   }
}

/* TODO: Fix the problem with secure_malloc() */
static passwd_conf_t *mem_alloc_passwd_t(void)
{
   /* Allocates and initialize the struct `passwd_conf_t` */
   //secure_malloc(passwd_conf_t *, new);

   return NULL;
}
