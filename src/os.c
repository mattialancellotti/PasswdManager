#define _XOPEN_SOURCE 500

#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>

#include <pass/defs.h>
#include <pass/os.h>

/* TODO:
 *    - Should probably extract them into a separate file
 */
static char *open_f(const char* /*f_name*/);
static int close_f(const char* /*addr*/);

char *users_path(void)
{
   /* Getting user's uid to search its default $HOME directory */
   struct passwd *user_info = getpwuid(getuid());

   /* TODO:
    *    - Move the warning message to the main function
    */
   fatal_err(user_info, NULL, "Couldn't find the user", NULL);

   /* Saving the home directory of the current user */
   char *home_dir = strdup(user_info->pw_dir);

   /* Returning the home_dir variable */
   return home_dir;
}

static char *open_f(const char *f_name)
{
   /* Defining file infors like its descriptor and all the other information */
   struct stat f_infos;
   int fd;

   /* Checking the file's name */
   fatal_err(f_name, NULL, "File's name is null", NULL);

   /* open() is a POSIX syscall. Take a look at open(2) in the man pages */
   /* TODO Might wanna add those  flags mentioned in the init_io function */
   fd = open(f_name, O_RDONLY);
   fatal_err(fd, -1, "File descriptor not set", NULL);

   /* Grabbing infos about the file and mapping it */
   fatal_err(fstat(fd, &f_infos), -1, 
         "Error while retrieving file's infos", NULL);

   char *content = 
      mmap(NULL, f_infos.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
   fatal_err(content, MAP_FAILED, "Couldn't read the file", NULL);

   /* Returning a pointer to the content of the file */
   return content;
}

static int close_f(const char *addr)
{
   /* Checking the file's name */
   fatal_err(addr, NULL, "File's name is null", 0);

   /* Unmapping the file */
   //munmap(addr, 
   
   return 0;
}
