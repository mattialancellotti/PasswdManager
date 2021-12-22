#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pass/defs.h>
#include <pass/services.h>
#include <pass/os.h>
#include <pass/term.h>

/*
 * This function takes a @service_name and concatenates it with the full
 * database path provided in include/pass/defs.h in pieces.
 *
 * @return The full path of the service, NULL otherwise.
 */
static char *service_absolute(const char* /*service_name*/);

/*
 * TODO:
 *  - Error list;
 */
int create_service(const char *service_name)
{
   prog_err((service_name == NULL), "Specify a valid service.", return -1);
   
   int err = 0;
   char *service_path = service_absolute(service_name);
   const char *msg = "The service already exists, "
                     "are you sure you want to overwrite it? [y/N] ";

   /* Checking the existance of the specified service */
   if (exists(service_path)) {
      if (ask_confirmation(msg) == 'n') {
         printf("Creation of the service '%s' interrupted.\n", service_name);

         free(service_path);
         return 1;
      }
   }

   /* Opens the file pointed by db_service (created if non-existing) */
   file_t *new_service = mcreate(service_path);
   if (new_service != NULL) {
      if (!mclose(new_service))
         printf("Service '%s' was successfully created.\n", service_name);
   }

   free(service_path);
   return 0;
}

int append_service(const char *service_name, const char *content)
{
   prog_err((service_name == NULL), "Specify a valid service.", return -1);
   prog_err((content == NULL), "Specify a valid content.", return -1);
   
   char *service_path = service_absolute(service_name);
   const char *msg = "The specified service doesn't exists yet.\n"
                     "Would you like to create it now? [y/N] ";

   /* 
    * Important: checks if the service exists (don't wanna do this in the main
    * file since it causes a lot output (code)).
    */
   if (!exists(service_path)) {
      if (ask_confirmation(msg) == 'n') {
         printf("Creation of the service '%s' interrupted.\n", service_name);
         free(service_path);
         return -1;
      }

      if (create_service(service_name) == 1) {
         free(service_path);
         return 1;
      }
   }

   /* Opening the file */
   file_t *service_file = mopen(service_path);
   if (service_file != NULL) {
      /* Writing the content to da service */
      if (cwrite(service_file->fd, content, 0) != -1) {
         if (mclose(service_file) != -1)
            printf("New password configured for service %s.\n", service_name);
         else
            fprintf(stderr, "Couldn't close the service.\n");
      } else
         fprintf(stderr, "Couldn't update the service.\n");
   } else
      fprintf(stderr, "Couldn't open the service.\n");

   free(service_path);
   return 0;
}

int delete_service(const char *service_name)
{
   prog_err((service_name == NULL), "Specify a valid service.", return -1);

   /* Checking if the service exists */
   char *service_path = service_absolute(service_name);
   if (!exists(service_path)) {
      fprintf(stderr, "The specified service '%s',"
                      " doesn't really exists.\n", service_name);

      free(service_path);
      return -1;
   }

   /* Deleting the actual file */
   int cerr = rmfile(service_path);
   prog_err((cerr == -1), "Couldn't delete the file.", );

   free(service_path);
   return 0;
}

int expose_service(const char *service_name)
{
   prog_err((service_name == NULL), "Specify a valid service.", return -1);

   /* Checking if the service exists */
   char *service_path = service_absolute(service_name);
   if (!exists(service_path)) {
      fprintf(stderr, "The specified service doesn't exists.\n");
      free(service_path);
      return -1;
   }

   /* Opeing the file to read its content (password) */
   file_t *file = mopen(service_path);
   if (file != NULL) {
      printf("Password: %s\n", file->file_content);
      if (mclose(file))
         fprintf(stderr, "Couldn't close the service.\n");
   } else
      fprintf(stderr, "Couldn't read the service.");

   return 0;
}

static char *service_absolute(const char *service_name)
{
   prog_err((service_name == NULL), "Specify a valid service.", return NULL);
   prog_err((program_db == NULL), "db_path is NULL.", return NULL);

   /* Creating the service */
   char *db_service = malloc(strlen(program_db) + strlen(service_name) + 1);
   db_service = strcat(strcpy(db_service, program_db), service_name);

   return db_service;
}
