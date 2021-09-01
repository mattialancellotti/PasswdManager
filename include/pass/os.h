#ifndef OS_H
#define OS_H

/* This struct rapresent a file */
typedef struct {
   char *file_content;
   int fd;
} file_t;

/* TODO doc */
char *users_input(void);

/*
 * This function searches for the $HOME directory of the user calling the
 * program. If not found a NULL is returned
 *
 * The caller needs to free the returned pointer.
 */
char *users_path(void);

/*
 * This function accepts a relative file path like `./local/share` and attaches
 * to it the full system path. For instance `.local/share` will be expanded into
 * its full directory /home/user/.local/share (I guess it also depends on the
 * file system).
 *
 * It returns the full path.
 */
char *absolute_path(const char* /*file_name*/);

/*
 * This function accepts 2 different string, `path` is the relative path of
 * non-existing directories and `absolute_path` is the location chosen to put
 * those directory in.
 */
int mkpath(const char* /*path*/, const char* /*absolute_path*/);

int rmfile(const char* /*file_path*/);
int rmpath(const char* /*path*/);

/*
 * This function accepts the absolute path of a directory and counts all the
 * file that resides in it. If an error accurs -1 is returned, otherwise the
 * number of file and directories.
 */
int is_empty(const char* /*path*/);

int exists(const char* /*f_name*/);
/* TODO:
 *  - doc
 */

file_t *mcreate(const char* /*f_name*/);
file_t *mopen(const char* /*f_name*/);


/*
 * This function uses the `write` call to write down `content` to the file
 * identified by the file descriptor `fd`.
 */
int cwrite(const int /*fd*/, const char* /*content*/);

/*
 * This function accepts a file and closes all its resources, the file
 * descriptor using the `close` syscall and unmaps the content if it's present.
 *
 * Also before unmapping the content it's also synced to the disk through the
 * `msync` syscall.
 */
int mclose(file_t* /*file*/);

#endif
