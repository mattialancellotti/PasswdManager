#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pass/defs.h>
#include <pass/pwman.h>
#include <pass/os.h>
#include <pass/term.h>
#include <pass/crypto.h>

int pm_init_hash(const char *hash_file)
{
   char *passwd = NULL, *verification_passwd = NULL;
   char *hash = NULL;

   /* Ask the user the password twice to make sure it's the correct one. */
   /* TODO: Password validation */
   printf("Insert a password: ");
   passwd = ask_pass();

   printf("\nInsert the password again: ");
   verification_passwd = ask_pass();

   /* The passwords are not the same */
   if (strcmp(passwd, verification_passwd)) {
      free(passwd);
      free(verification_passwd);

      putchar('\n');
      return -1;
   }

   /*
    * If the user has entered a valid password that same password is being
    * hashed through the `hash_password` function in crypto.c
    */
   hash = hash_password(passwd);

   /* Writing the hash password to the file */
   file_t *file = mcreate(hash_file);
   int werr = cwrite(file->fd, hash, 0);
   int cerr = mclose(file);

   printf("\nThe password manager is good to go.\n");

   free(passwd);
   free(verification_passwd);
   free(hash);

   return 0;
}

char *pm_hash(const char *hash_file)
{
   char *actual_hash = NULL;
   file_t *hash;

   /* 
    * Reading the hash saved in `hash_file`. If for some reason an error occurs
    * or the file doesn't exist at all it returns NULL.
    */
   if ((hash = mopen(hash_file)) == NULL)
      return actual_hash;

   if (hash->file_content == NULL) {
      fprintf(stdout, "No password found. Use ezPass --init to initialize one.\n");
      goto hash_exit;
   }

   /* Saving the hash and destroying the hash file */
   actual_hash = strdup(hash->file_content);

hash_exit:
   /* Freeing the memory */
   mclose(hash);

   return actual_hash;
}

int pm_init_path(void)
{
   char *users_home = users_path();

   /*
    * Checks if both directories exist and if they do moves on the program,
    * otherwise they are created. If some error happens during the creation
    * this functions returns -1. (Look at `mkpath` in os.c)
    */
   printf("Checking paths\n");
   if (mkpath(PROG_ROOT ROOT_PATH PASS_DB, users_home)) {
      free(users_home);

      return -1;
   }

   free(users_home);
   return 0;
}

int pm_purge_db(const char *db)
{
   prog_err((db == NULL), "Specify a valid database.", return -1);

   if (rmpath(db) == -1) {
      fprintf(stderr, "Couldn't complete the purge of the database.\n");
      return -1;
   }

   return 0;
}
