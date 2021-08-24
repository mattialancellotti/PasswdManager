#define _XOPEN_SOURCE 500

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>

#include <pass/defs.h>
#include <pass/os.h>

/* TODO: doc */
static file_t *file_t_malloc(char* /*content*/, int /*fd*/);
static void file_t_free(file_t* /*fstruct*/);

char *users_path(void)
{
   /* Getting user's uid to search its default $HOME directory */
   struct passwd *user_info = getpwuid(getuid());
   exit_eq(user_info, NULL, NULL);

   /* Saving the home directory of the current user */
   char *home_dir = malloc(strlen(user_info->pw_dir) + 2);
   home_dir = strcpy(home_dir, user_info->pw_dir);
   home_dir = strcat(home_dir, "/");

   /* Returning the home_dir variable */
   return home_dir;
}

char *absolute_path(const char *file_name)
{
   /* Creating the complete path */
   char *home_dir = users_path();
   char *abs_path = malloc(strlen(home_dir) + strlen(file_name) + 1);
   abs_path = strcat(strcpy(abs_path, home_dir), file_name);

   /* This needs to be freed */
   free(home_dir);
   return abs_path;
}

int mkpath(const char *path, const char *absolute_path)
{
   /* Need a duplicate because strtok_r modifies the string */
   char *rwpath = strdup(path);
   char *token = NULL;

   /* Keeping track of path */
   char *complete_path = malloc(strlen(absolute_path) + (strlen(path)*2) + 1);
   complete_path = strcpy(complete_path, absolute_path);

   /* This pointer is needed because strtok_r changes the given one */
   char *tmp_ptr = rwpath;

   /* Thise while loops through all the pecies of the given path */
   while ((token = strtok_r(tmp_ptr, "/", &tmp_ptr))) {
      complete_path = strcat(complete_path, token);
      complete_path = strcat(complete_path, "/");

      /* Checks if the directory exists */
      if (open(complete_path, 0) == -1)
         if (mkdir(complete_path, S_IRWXU) == -1) {
            perror(complete_path);

            return -1;
         }
   }

   free(complete_path);
   free(rwpath);

   return 0;
}

int is_empty(const char *path)
{
   exit_eq(path, NULL, -1);

   int count = 0;
   DIR *path_stream = opendir(path);
   fatal_err(path_stream, NULL, "opendir", -1);

   /* TODO: errno checking (see RETURN VALUE in readdir.3 manual) */
   while (readdir(path_stream))
      count++;
   
   int err = closedir(path_stream);
   fatal_err(err, -1, "closedire", -1);

   /* Bot `.` and `..` are counted and to avoid this 2 is being subracted. */
   return count-2;
}

file_t *mcreate_open(const char *f_name)
{
   /* Checking the file's name */
   /* TODO: not correct use of */
   fatal_err(f_name, NULL, "File's name is null", NULL);

   /* Defining file infors like its descriptor and all the other information */
   struct stat inode;
   char *content = NULL;
   int fd, st;

   /* open() is a POSIX syscall. Take a look at open(2) in the man pages */
   /* TODO:
    *  - Might wanna add those flags mentioned in the init_io function;
    */
   fd = open(f_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
   fatal_err(fd, -1, "open", NULL);

   /* Grabbing infos about the file and mapping it */
   st = fstat(fd, &inode);
   fatal_err(st, -1, "fstat", NULL);

   /* Checking if the file is new/empty */
   if (inode.st_size == 0)
      return file_t_malloc(content, fd);

   /* Mapping the file */
   content = mmap(NULL, inode.st_size,
                  PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
   fatal_err(content, MAP_FAILED, "mmap", NULL);

   /* Returning a pointer to the file */
   return file_t_malloc(content, fd);
}

int cwrite(int fd, const char *content)
{
   exit_eq(content, NULL, -1);

   size_t clen = strlen(content);

   /* Writing stuff down */
   ssize_t werr = write(fd,  content, clen);
   exit_eq(werr, -1, -1);
   
   return 0;
}

int mclose(file_t *file)
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
