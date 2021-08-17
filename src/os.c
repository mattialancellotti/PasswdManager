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

static file_t *file_t_malloc(char* /*content*/, int /*fd*/);
static void file_t_free(file_t* /*fstruct*/);

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
   /* Checking the file's name */
   fatal_err(f_name, NULL, "File's name is null", NULL);

   /* Defining file infors like its descriptor and all the other information */
   struct stat f_infos;
   char *content = NULL;
   file_t *ofile = NULL;
   int fd, st;

   /* open() is a POSIX syscall. Take a look at open(2) in the man pages */
   /* TODO:
    *  - Might wanna add those  flags mentioned in the init_io function;
    */
   fd = open(f_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
   fatal_err(fd, -1, "open", NULL);

   /* Grabbing infos about the file and mapping it */
   st = fstat(fd, &f_infos);
   fatal_err(st, -1, "fstat", NULL);

   /* Checking if the file is new/empty */
   if (f_infos.st_size == 0) {
      /* Definin the new file */
      ofile = file_t_malloc(content, fd);

      return ofile;
   }

   /* Mapping the file */
   /* TODO:
    *  - Close file descriptor before exiting;
    */
   content = mmap(NULL, f_infos.st_size,
                  PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
   fatal_err(content, MAP_FAILED, "mmap", NULL);

   /* Definin the new file */
   ofile = malloc(sizeof(file_t));
   ofile->file_content = content;
   ofile->fd = fd;

   /*
   exec_if((fd != -1), 
         warn_user((close(fd) == -1), "Couldn't close the file"));
         */

   /* Returning a pointer to the file */
   return ofile;
}

int os_fwrite(int fd, const char *content)
{
   exit_eq(content, NULL, -1);

   size_t clen = strlen(content);

   /* Writing stuff down */
   ssize_t werr = write(fd,  content, clen);
   exit_eq(werr, -1, -1);
   
   return 0;
}

int os_fclose(file_t *file)
{
   /* Checking if the file is not null */
   exit_eq(file, NULL, -1);

   /* Closing the file descriptor */
   int desc = close(file->fd);
   fatal_err(desc, -1, "close", -1);

   /* This is only needed for unmapping the file */
   if (file->file_content == NULL)
      goto close_exit;

   /* Syincing the content of the file */
   size_t len = strlen(file->file_content);
   int sync = msync(file->file_content, len, MS_SYNC|MS_INVALIDATE);
   fatal_err(sync, -1, "msync", -1);

   /* Unmapping the file */
   int umap = munmap(file->file_content, len);
   fatal_err(umap, -1, "munmap", -1);


   /* Freeing the structure */
close_exit:
   free(file);

   /* Returns successfully */
   return 0;
}

static file_t *file_t_malloc(char *content, int fd)
{
   /* Allocating memory for the new struct */
   file_t *new_file = malloc(sizeof(file_t));
   new_file->file_content = content;
   new_file->fd = fd;

   return new_file;
}

static void file_t_free(file_t *fstruct)
{
   exit_eq(fstruct, NULL,);

   if (fstruct->file_content != NULL)
      free(fstruct->file_content);

   free(fstruct);
}
