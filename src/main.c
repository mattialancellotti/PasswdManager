/*
 * TODO:
 *    # Urgent
 *    - Include profiles
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
 *    - Default action will be to ask a password and to print out the name of
 *      the profiles.
 *    - Save passwords in an XML encrypted file.
 *
 *    # Configuration file
 *    - settings.yaml
 *       login_pass_hashing :: [ md5, sha256, sha128 ]
 *       password_encryption :: [ .. ]
 *       passwds_file :: !!str `path`
 *       accept_same_passwd :: !!boolean `true|false`
 *       profiles_enabled :: !!boolean `true|false`
 *       colors :: !!boolean `true|false`
 *       passwd_min_len :: !!int `length`
 *       passwd_max_len :: !!int `length`
 *    - profiles.yaml
 *       length :: !!int `length`
 *       name :: !!str `name`
 *       desc :: !!str `description`
 *       nchars :: [ u_char, digit, l_char, sign ]
 *
 */
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

#include <pass/storage.h>
#include <pass/utils.h>
#include <pass/args.h>
#include <pass/term.h>

/* TODO doc */
static void mem_dealloc_passwd_t(passwd_t** /*passwd_s*/, const size_t /*length*/);
static passwd_t* mem_alloc_passwd_t(void);

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

   char *passwd = ask_pass();
   printf("Password: %s", passwd);
   free(passwd);
   

   return 0;
   /*
    * TODO
    *    Ask for a password key to decrpyt files.
    *    This is to be done here so that there won't be any kind of useless load
    *    if the user enters the wrong password.
    */
   if (load_config()) {
      fprintf(stderr, "Couldn't initiate the saving file.");
      return EXIT_FAILURE;
   }

   /* 
    * This struct rapresent a configuration block with the properties of a
    * password. The generated password should follow those properties.
    *
    * It has default options useful to the `handle_args` function.
    */
   /*
    * TODO
    *    - Needs to be updated by following what happens in the options array
    *    - Might have to refactor this one since profiles are gonna take over.
    *      An idea is that passwd_conf_t will represent the configuration for
    *      the program that will store options such as the saving_point, export
    *      functionalities, algorithms and formatting options.
    */
   passwd_conf_t config_file = { 
      .saving_point = NULL,
      .length = DEFAULT_PASSWD_SIZE,
      .times  = 1,
      .char_not_admitted = 0,
      .saving_functionality = 1
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

   /* 
    * TODO
    *    Might want to extract this and put it into a function to create a more
    *    flexible piece of code.
    */
   /* Allocating the array of passwords */
   char *passwords[config_file.times];

   //passwd_profile_t *passwords[config_file.times]

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

/* Safely deallocates a password */
static void mem_dealloc_passwd_t(passwd_t ** passwd_s, const size_t length)
{
   /* Some controls to avoid segmatation faults */
   if (*passwd_s) {
      if ((*passwd_s)->passwd) {
         /* 
          * Clears out the password to avoid it being stolen even after the
          * deallocation.
          */
#if defined(__libsodium__)
         sodium_memzero((*passwd_s)->passwd, length);
#else
	 memset((*passwd_s)->passwd, 0, length);
	 free((*passwd_s)->passwd);
#endif
	 (*passwd_s)->passwd = NULL;
      }

      free(*passwd_s);
      *passwd_s = NULL;
   }
}

static passwd_t *mem_alloc_passwd_t(void)
{
   /* Allocates and initialize the struct `passwd_t`, use sizeof(*new) understand */
#if defined(__libsodium__)
   passwd_t *new = sodium_malloc(sizeof(passwd_t));
   sodium_memzero(new, 56);
#else
   passwd_t *new = malloc(sizeof(passwd_t));
   memset(new, 0, 56);
#endif

   return new;
}

void check_passwd(passwd_t ** const passwd_created, const size_t length) {
	if (!(*passwd_created) && !((*passwd_created)->passwd))
		return;

	enum TYPE last_c = NONE;
	char already_used[length];
	size_t arr_i=0, res=0;
	char tmp;

	for (size_t i=0; i<length; i++) {
		if (!(res = search(already_used, arr_i, (tmp=(*passwd_created)->passwd[i]))))
			already_used[++arr_i] = tmp;

		if (isdigit(tmp)) {
			if (last_c == DIGIT)
				(*passwd_created)->consecutive_digit++;
			else
				last_c = DIGIT;

			(*passwd_created)->number_digit++;

			if (res)
				(*passwd_created)->repeated_digit++;
		} else if (islower(tmp)) {
			if (last_c == L_CHAR)
				(*passwd_created)->consecutive_l_char++;
			else
				last_c = L_CHAR;

			(*passwd_created)->number_l_char++;

			if (res)
				(*passwd_created)->repeated_l_char++;
		} else if (isupper(tmp)) {
			if (last_c == U_CHAR)
				(*passwd_created)->consecutive_u_char++;
			else
				last_c = U_CHAR;

			(*passwd_created)->number_u_char++;

			if (res)
				(*passwd_created)->repeated_u_char++;
		} else {
			if (last_c == SIGN)
				(*passwd_created)->consecutive_sign++;
			else
				last_c = SIGN;

			(*passwd_created)->number_sign++;

			if (res)
				(*passwd_created)->repeated_sign++;
		}
	}
}

/* You really have to make this look better */
void print(const passwd_t * const passwd_info) {
	printf("\n%s===CONSECUTIVE CHARACTERS===%s\n", RED, RESET);
	printf("\t| Consecutive upper case chars: %d\n\t| Consecutive lower case chars: %d\n\t| Consecutive digits: %d\n\t| Consecutive signs: %d\n", passwd_info->consecutive_u_char, passwd_info->consecutive_l_char, passwd_info->consecutive_digit, passwd_info->consecutive_sign);
	printf("\n%s===REPEATED CHARACTERS===%s\n", RED, RESET);
	printf("\t| Repeated upper case chars: %d\n\t| Repeated lower case chars: %d\n\t| Repeated digits: %d\n\t| Repeated signs: %d\n", passwd_info->repeated_u_char, passwd_info->repeated_l_char, passwd_info->repeated_digit, passwd_info->repeated_sign);
	printf("\n%s===NUMBER OF EACH TYPE===%s\n", RED, RESET);
	printf("\t| Number of upper case chars: %d\n\t| Number of lower case chars: %d\n\t| Number of digits: %d\n\t| Number of signs: %d\n", passwd_info->number_u_char, passwd_info->number_l_char, passwd_info->number_digit, passwd_info->number_sign);
}
