#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pass/defs.h>
#include <pass/main.h>
#include <pass/pwman.h>
#include <pass/os.h>
#include <pass/term.h>
#include <pass/crypto.h>

int pm_init_hash(const char *hash_file)
{
   char *passwd = NULL, *verification_passwd = NULL;
   char *hash;

   /* Ask the user the password twice to make sure it's the correct one. */
   /* TODO: Password validation */
   printf("Insert a password: ");
   passwd = ask_pass();

   printf("\nInsert the password again: ");
   verification_passwd = ask_pass();

   /* The passwords are not the same */
   if (strcmp(passwd, verification_passwd))
      return -1;

   /*
    * If the user has entered a valid password that same password is being
    * hashed through the `hash_password` function in crypto.c
    */
   hash = hash_password(passwd);

   /* Writing the hash password to the file */
   file_t *file = mopen(hash_file);
   int werr = cwrite(file->fd, hash);
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
      goto hash_exit;

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
   /* Important program paths */
   char *program_root = absolute_path(PROG_ROOT);
   char *program_path = absolute_path(PROG_ROOT ROOT_PATH);

   /*
    * Checks if both directories exist and if they do moves on the program,
    * otherwise they are created. If some error happens during the creation
    * this functions returns -1. (Look at `mkpath` in os.c)
    */
   if (mkpath(ROOT_PATH, program_root) || mkpath(PASS_DB,   program_path))
      return -1;

   /* Freeing stuff */
   free(program_root);
   free(program_path);

   return 0;
}

int pm_create_service(const char *service_name)
{
   prog_err((service_name == NULL), "Specify a valid service.", return -1);
   const char *msg = "The service already exists, "
                     "are you sure you want to overwrite it? [y/N] ";
   int exit_status = 0;

   /* Creating the service */
   char *program_db = absolute_path(PROG_ROOT ROOT_PATH PASS_DB);
   char *db_service = malloc(strlen(program_db) + strlen(service_name) + 1);
   db_service = strcpy(db_service, program_db);
   db_service = strcat(db_service, service_name);

   /* Checking the existance of the specified service */
   if (!exists(service_name)) {
      if (ask_confirmation(msg) == 'n') {
         printf("Creation of the service '%s' interrupted.\n", service_name);
         exit_status = 1;
         goto no_service;
      }
   }

   /* Opens the file pointed by db_service (created if non-existing) */
   file_t *service_file = mcreate(db_service);
   exit_if((service_file == NULL), -1);

   int err = mclose(service_file);
   exit_if((err == -1), -1);
   printf("Service '%s' was successfully created.\n", service_name);

   /* Returning successfully */
no_service:
   free(program_db);
   free(db_service);

   return exit_status;
}
