#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>

#include <pass/defs.h>
#include <pass/os.h>

/*
 * All the functions regarding file input/output work with a strut that
 * rapresents a open file. For this reason an allocation function is used to
 * clear the code up.
 */
static file_t *file_t_malloc(char* /*content*/, int /*fd*/);

/* 
 * If something is being allocated elegantly with its own function, the
 * deallocation deserves a function too right?
 */
static void file_t_free(file_t* /*fstruct*/);

char *users_input(void)
{
   char *buffer = NULL;
   size_t buffer_size = 20;

   /* Reading the user's input the chad way */
   ssize_t len = getline(&buffer, &buffer_size, stdin);
   system_err((len == -1), "getline", NULL);

   return buffer;
}

char *users_path(void)
{
   /* Getting user's uid to search its default $HOME directory */
   struct passwd *user_info = getpwuid(getuid());
   system_err((user_info == NULL), "getpwuid", NULL);

   /* Saving the home directory of the current user */
   char *home_dir = malloc(strlen(user_info->pw_dir) + 2);
   home_dir = strcpy(home_dir, user_info->pw_dir);
   home_dir = strcat(home_dir, "/");

   /* Returning the home_dir variable */
   return home_dir;
}

char *absolute_path(const char *file_name)
{
   prog_err((file_name == NULL), "Needs a valid file name.", return NULL);

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
   char *complete_path = malloc(strlen(absolute_path) + strlen(path) + 1);
   complete_path = strcpy(complete_path, absolute_path);

   /* This pointer is needed because strtok_r changes the given one */
   char *tmp_ptr = rwpath;

   /* This while loops through all the pecies of the given path */
   while ((token = strtok_r(tmp_ptr, "/", &tmp_ptr))) {
      printf("Creating %s - %s\n", token, complete_path);

      complete_path = strcat(complete_path, token);
      complete_path = strcat(complete_path, "/");

      /* Checks if the directory exists */
      if (open(complete_path, 0) == -1) {
         if (mkdir(complete_path, S_IRWXU) == -1) {
            perror(complete_path);

            return -1;
         }

         printf("%s created\n", complete_path);
      } else
         printf("%s\n", complete_path);
   }

   free(complete_path);
   free(rwpath);

   return 0;
}

int rmfile(const char *file_path)
{
   prog_err((file_path == NULL), "The specified file is not valid.", return -1);
   system_err((unlink(file_path) == -1), "unlink", -1);

   return 0;
}

int rmpath(const char *path)
{
   prog_err((path == NULL), "The specified path is not valid.", return -1);
   struct dirent *file_stream;

   /* Opening a stream to the directory */
   DIR *path_stream = opendir(path);
   system_err((path_stream == NULL), "opendir", -1);

   errno = 0;
   while ((file_stream = readdir(path_stream))) {
      /* Checking if the navigation directories are included */
      if (!strcmp(file_stream->d_name, ".") || !strcmp(file_stream->d_name, ".."))
         continue;

      char *tmp = malloc(strlen(path) + strlen(file_stream->d_name) + 1);
      tmp = strcat(strcpy(tmp, path), file_stream->d_name);

      /* Checking file type and deleting it */
      switch (file_stream->d_type) {
      case DT_UNKNOWN: return -1;    
      case DT_DIR:
         prog_err((rmpath(tmp) == -1), "Couldn't complete the operation.", return -1);
         break;
      case DT_REG:
         system_err((unlink(tmp) == -1), "remove", -1);
         break;
      }

      ifdef_free(tmp);
   }

   /* Closing the file stream */
   system_err((errno != 0), "readdir", -1);
   system_err((closedir(path_stream) == -1), "closedir", -1);
   
   /* Deleting the root dir */
   system_err((rmdir(path) == -1), "rmdir", -1);
   return 0;
}

int is_empty(const char *path)
{
   prog_err((path == NULL), "The specified path is not valid.", return -1);

   int count = 0;
   struct dirent *file_stream;

   /* Opening a stream to the directory */
   DIR *path_stream = opendir(path);
   system_err((path_stream == NULL), "opendir", -1);

   /* Listing files */
   errno = 0;
   while ((file_stream = readdir(path_stream)))
      count++;
   
   /* 
    * Since `readdir` exits with NULL when there are no files as well as when
    * theres is an error, this macro checks if an errno was set and act
    * consequently.
    *
    * Also `file_stream` is a statically allocated struct that does not need to
    * be deallocated.
    */
   system_err((errno != 0), "readdir", -1);

   /* Closing the file stream and checking for final errors */
   system_err((closedir(path_stream) == -1), "closedir", -1);

   /* Both `.` and `..` are counted and to avoid this 2 is being subracted. */
   return count-2;
}

int exists(const char *f_name)
{
   prog_err((f_name == NULL), "Specify a valid file name.", return -1);

   /* 
    * This call is going to try to open `f_name`. By doing
    * this the function will try to create the file and if it's already there
    * it's going to return an errno set to EEXIST.
    */
   file_t *tmp_file = mopen(f_name);
   if (tmp_file == NULL && errno == ENOENT)
      return 0;

   mclose(tmp_file);
   return 1;
}

file_t *mcreate(const char *f_name)
{
   prog_err((f_name == NULL), "Specify a valid file name.", return NULL);

   /* Tries to create it */
   int fd = creat(f_name, S_IRUSR|S_IWUSR);
   system_err((fd == -1), "creat", NULL);

   /* Returning the new file */
   return file_t_malloc(NULL, fd);
}

file_t *mopen(const char *f_name)
{
   /* Checking the file's name */
   prog_err((f_name == NULL), "Specify a valid file name.", return NULL);

   /* Defining file infors like its descriptor and all the other information */
   struct stat inode;
   char *content = NULL;
   int fd, st;

   /* open() is a POSIX syscall. Take a look at open(2) in the man pages */
   fd = open(f_name, O_RDWR, S_IRUSR | S_IWUSR);
   if (fd == -1)
      /* This is not using system_err enymore because I need it to be siltent */
      return NULL;

   /* Grabbing infos about the file and mapping it */
   st = fstat(fd, &inode);
   system_err((st == -1), "fstat", NULL);

   /*
    * This is needed because the `open` syscall could also have created the file
    * passed to the function. By doing so the mapping function `mmap` won't work
    * because it basically tries to map an empty region.
    */
   if (inode.st_size == 0)
      return file_t_malloc(content, fd);

   /* Mapping the file */
   content = mmap(NULL, inode.st_size,
                  PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
   system_err((content == MAP_FAILED), "mmap", NULL);

   /* Returning a pointer to the file */
   return file_t_malloc(content, fd);
}

int cwrite(int fd, const char *content, size_t len)
{
   prog_err((content == NULL), "No actual content to write.", return -1);

   if (len == 0)
      len = strlen(content);

   /* Writing stuff down */
   ssize_t werr = write(fd,  content, len);
   system_err((werr == -1), "write", -1);
   
   return 0;
}

int mclose(file_t *file)
{
   /* Checking if the file is not null */
   prog_err((file == NULL), "The specified file doesn't exists.", return -1);

   /* Closing the file descriptor */
   int desc = close(file->fd);
   system_err((desc == -1), "close", -1);

   /* This is only needed for unmapping the file */
   if (file->file_content == NULL)
      goto close_exit;

   /* Syncing the content of the file */
   size_t len = strlen(file->file_content);
   int sync = msync(file->file_content, len, MS_SYNC|MS_INVALIDATE);
   system_err((sync == -1), "msync", -1);

   /* Unmapping the file */
   int umap = munmap(file->file_content, len);
   system_err((umap == -1), "munmap", -1);


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
   exit_if((fstruct == NULL),);

   if (fstruct->file_content != NULL)
      free(fstruct->file_content);

   free(fstruct);
}
