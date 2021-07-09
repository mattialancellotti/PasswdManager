#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

size_t search(const char *, const size_t, const char);

/* 
 * Just prints out the help message.
 * It accepts an unsigned decimal that acts as a flag, if the second bit is 1
 * it will also prints the version of the program.
 */
void help();
size_t count_digits(float);

#endif
