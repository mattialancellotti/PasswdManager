#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pass/defs.h>
#include <pass/pwman.h>
#include <pass/os.h>
#include <pass/term.h>
#include <pass/crypto.h>

int pm_init(const char *hash_file)
{
   char *passwd = NULL, *verification_passwd = NULL;
   char *hash;

   /* Asking the password */
   printf("Insert a password: ");
   passwd = ask_pass();

   printf("\nInsert the password again: ");
   verification_passwd = ask_pass();

   /* The passwords are not the same */
   if (strcmp(passwd, verification_passwd))
      return -1;

   /* Creating the hash */
   hash = hash_password(passwd);

   /* Writing the hash password to the file */
   /* TODO:
    *  - file->fd could be null;
    */
   file_t *file = os_fopen_rw(hash_file);
   int werr = os_fwrite(file->fd, hash);
   int cerr = os_fclose(file);

   free(passwd);
   free(verification_passwd);
   free(hash);

   return 0;
}

char *pm_hash(const char *hash_file)
{
   /* Trying to read hash file */
   char *actual_hash = NULL;
   file_t *hash;
   if ((hash = os_fopen_rw(hash_file)) == NULL)
      goto hash_exit;

   if (hash->file_content == NULL) {
      fprintf(stdout, "No password found. Use ezPass --init to initialize one.");
      goto hash_exit;
   }

   /* Saving the hash and destroying the hash file */
   actual_hash = strdup(hash->file_content);

hash_exit:
   /* Freeing the memory */
   os_fclose(hash);

   return actual_hash;
}

int pm_init_path(void)
{
   /* Important program paths */
   char *program_root = absolute_path(PROG_ROOT);
   char *program_path = absolute_path(PROG_ROOT ROOT_PATH);

   /* Creating paths */
   if (mkpath(ROOT_PATH, program_root) || mkpath(PASS_DB,   program_path))
      return -1;

   /* Freeing stuff */
   free(program_root);
   free(program_path);

   return 0;
}
