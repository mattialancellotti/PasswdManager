#define _XOPEN_SOURCE 500

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>

#include <pass/defs.h>
#include <pass/os.h>

char *users_path(void)
{
   /* Getting user's uid to search its default $HOME directory */
   struct passwd *user_info = getpwuid(getuid());
   exit_eq(user_info, NULL, NULL);

   /* Saving the home directory of the current user */
   char *home_dir = strdup(user_info->pw_dir);

   /* Returning the home_dir variable */
   return home_dir;
}

file_t *os_fopen_rw(const char *f_name)
{
   /* Defining file infors like its descriptor and all the other information */
   struct stat f_infos;
   int fd;

   /* Checking the file's name */
   fatal_err(f_name, NULL, "File's name is null", NULL);

   /* open() is a POSIX syscall. Take a look at open(2) in the man pages */
   /* TODO Might wanna add those  flags mentioned in the init_io function */
   fd = open(f_name, O_RDWR);
   fatal_err(fd, -1, "File descriptor not set", NULL);

   /* Grabbing infos about the file and mapping it */
   fatal_err(fstat(fd, &f_infos), -1, 
         "Error while retrieving file's infos", NULL);

   char *content = 
      mmap(NULL, f_infos.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
   fatal_err(content, MAP_FAILED, "Couldn't read the file", NULL);

   /* Definin the new file */
   /* TODO: Fix with secure_malloc */
   file_t *ofile = malloc(sizeof(file_t));
   ofile->file_content = content;
   ofile->fd = fd;

   /* Returning a pointer to the file */
   return ofile;
}

int os_fclose(file_t *file)
{
   /* Checking if the file is not null */
   exit_eq(file, NULL, -1);
   exit_eq(file->file_content, NULL, -1);

   /* Syincing the content of the file */
   size_t len = strlen(file->file_content);
   int sync = msync(file->file_content, len, MS_SYNC|MS_INVALIDATE);
   fatal_err(sync, -1, "Couldn't sync the file", -1);

   /* Unmapping the file */
   int umap = munmap(file->file_content, len);
   fatal_err(umap, -1, "Couldn't unmap the file", -1);

   /* Closing the file descriptor */
   int desc = close(file->fd);
   fatal_err(desc, -1, "Couldn't close the file desc.", -1);

   /* Freeing the structure */
   free(file);

   /* Returns successfully */
   return 0;
}
