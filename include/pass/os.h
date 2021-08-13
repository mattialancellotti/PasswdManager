#ifndef OS_H
#define OS_H

/* This struct rapresent a file */
typedef struct {
   char *file_content;
   int fd;
} file_t;

/*
 * This function searches for the $HOME directory of the user calling the
 * program. If not found a NULL is returned
 *
 * The caller needs to free the returned pointer.
 */
char *users_path(void);

/* TODO:
 *  - doc
 */
file_t *os_fopen_rw(const char* /*f_name*/);
int os_fwrite(const int /*fd*/, const char* /*content*/);
int os_fclose(file_t* /*file*/);

#endif
