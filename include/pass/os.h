#ifndef OS_H
#define OS_H

/*
 * This function searches for the $HOME directory of the user calling the
 * program. If not found a NULL is returned
 *
 * The caller needs to free the returned pointer.
 */
char *users_path(void);

#endif
