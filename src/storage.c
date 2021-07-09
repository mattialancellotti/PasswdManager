#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>

#include <pass/storage.h>
#include <pass/yaml_parser.h>

static char *map_file(const char* /*f_name*/);

/* TODO Should return a structure config_t */
config_t *load_config(void)
{
   size_t len;

   /* Getting the user's uid to search its default $HOME directory */
   struct passwd *home_dir = getpwuid(getuid());
   if (home_dir == NULL)
      return NULL;

   len = strlen(home_dir->pw_dir);

   /* 
    * List of settings files saved in read order.
    * A file is overridden by its successor.
    */
   /* TODO
    *    - Add profiles configuration files
    *    - Might want to create a struct for these kinds of files since they
    *      could have modes (take a look at umask(2))
    *    - Add samples in /usr of configuration so that the program can run
    *      without those settings
    */
   char local_file[strlen("/.ezPass") + len];
   char conf_file[strlen("/.config/ezPass/settings.yaml") + len];
   
   char *settings_files[3] = {
      "/etc/ezPass/settings.yaml",
      strcat(strcpy(local_file, home_dir->pw_dir), "/.ezPass"),
      strcat(strcpy(conf_file,  home_dir->pw_dir), "/.config/ezPass/settings.yaml")
   };

   /*
   for (size_t i = 0; i < 3; i++) {
      if (file = map_file(settings_files[i])) {
         break;
      }
   }
   */

   /* Don't need to map the file, libyaml exposes a function to read a file */
   char *file = map_file(settings_files[0]);
   parse_settings(file);

   /* 
    * TODO
    *    - Should add the load-configs here so that this function can initiate
    *      everything without any kind of problems.
    *    - If configuration files are not found then bad things happen.
    *    - If the password file is not found then nothing happen.
    */

   //free(file);
   return 0;
}

/*
 * This function is used to open files by mapping them in memory instead of
 * using stdio functions like fprintf/fopen. This is done by include sys/mman.h
 * and some other headers that make your life easier lie sys/stat.h for file size
 */
static char *map_file(const char *f_name)
{
   /* Defining file infors like its descriptor and all the other information */
   struct stat f_infos;
   int fd;

   /* Checking the file's name */
   /* TODO Replace those kind of things with a macro */
   if (f_name == NULL) {
      perror("File's name is null");
      return NULL;
   }

   /* open() is a POSIX syscall. Take a look at open(2) in the man pages */
   /* TODO Might wanna add those  flags mentioned in the init_io function */
   fd = open(f_name, O_RDONLY);

   /* Checking for errors during file operation */
   if (fd == -1) {
      perror("File descriptor not set");
      return NULL;
   }

   /* Grabbing infos about the file and mapping it */
   if (fstat(fd, &f_infos) == -1) {
      perror("Error while retrieving file's infos");
      return NULL;
   }
   char *f_content = mmap(NULL, f_infos.st_size, PROT_READ, MAP_SHARED, fd, 0);

   /* Returning a pointer to the content of the file */
   return f_content;
}
