#ifndef OS_H
#define OS_H

/* This struct rapresent a file */
typedef struct {
   char *file_content;
   int fd;
} file_t;

/*
 * Asks for the user's input from stdin.
 *
 * @return User's input line.
 */
char *users_input(void);

/*
 * This function searches for the $HOME directory of the user calling the
 * program. If not found a NULL is returned
 *
 * @return The user's home directory (like /home/mattia).
 * @warn   The returned pointer eeds to be freed by the caller.
 */
char *users_path(void);

/*
 * This function accepts a relative file path like `./local/share` as
 * @file_name and attaches to it the full system path. For instance
 * `.local/share` will be expanded into its full directory /home/user/.local/share
 * (I guess it also depends on the file system).
 *
 * @return The absolute path of @file_name.
 * @warn The returned pointer needs to be freed by the caller.
 */
char *absolute_path(const char* /*file_name*/);

/*
 * This function accepts 2 different strings:
 *    - @path is the relative path of non-existing directories 
 *    - @absolute_path is the location chosen to put those directory in.
 * For instance this functions acts as if mkdir -p was called.
 *
 * @return A status flag that is 0 on success and -1 otherwise.
 */
int mkpath(const char* /*path*/, const char* /*absolute_path*/);

/*
 * This function accepts a single string @file_path that is the path of the
 * file that needs to be deleted (Yeah taht simple who would have thought?).
 *
 * @return A status flag that is 0 on success and -1 otherwise.
 */
int rmfile(const char* /*file_path*/);

/*
 * This function is the exact opposite of mkpath (basically a rm -r).
 * It accepts a @path that needs to be deleted.
 *
 * @return A status flag that is 0 on success and -1 otherwise.
 */
int rmpath(const char* /*path*/);

/*
 * This function accepts the absolute path (as @path) of a directory and counts 
 * all the files in it. 
 *
 * @return If this function succeeds the number of files found is returned.
 *         If an error occurs -1 is returned.
 */
int is_empty(const char* /*path*/);

/*
 * This function accepts an absolute path that points to a file (as @f_name) and
 * checks its existance.
 *
 * @return A status flag that can be either 0 or 1 whether the file was found or
 *         not. If an error occurs a -1 is returned.
 */
int exists(const char* /*f_name*/);

/*
 * This function accepts an absolute path (as @f_name) that points to a file
 * that needs to be created. It uses creat that is an alias of the open system
 * calls but having 2 different functions to handle this made the writing a lot
 * easier.
 *
 * @return A struct of type file_t that only stores the file descriptor.
 *         If an error occurs -1 is returned.
 * @warn The returned struct needs to be freed, closed or whatever the fuck
 *       using the mclose function (see below);
 */
file_t *mcreate(const char* /*f_name*/);

/*
 * This function accepts a full-path file name (as @f_name) and opens it using
 * the open system call (see man open.2). Than it uses the mmap function to map
 * the content of the file to a buffer (see man mmap.2). This function does not
 * create files, use mcreate for that.
 *
 * @return A struct of type file_t that stores both the file descriptor and the
 *         content of the file.
 *         If an error occurs -1 is returned.
 * @warn The returned struct needs to be freed, closed or whatever the fuck
 *       using the mclose function (see below);
 */
file_t *mopen(const char* /*f_name*/);


/*
 * This function accepts a buffer (as @content) and writes that down to the
 * file identified by the file descriptor (as @fd).
 *
 * @return A status flag that is 0 on success and -1 if something bad happened.
 */
int cwrite(const int /*fd*/, const char* /*content*/);

/*
 * This function accepts a file struct (as @file) and closes/frees all its
 * resources, the file descriptor using the `close` syscall and unmaps the
 * content if it's present.
 *
 * Also before unmapping the content it's also synced to the disk through the
 * `msync` syscall.
 *
 * @return A status flag that is 0 on success and -1 if something bad happened.
 */
int mclose(file_t* /*file*/);

#endif
