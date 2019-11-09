#ifndef UTILS_H
#define UITLS_H

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_NOT_ADMITTED_STR_NUM 4

unsigned check_num(const char *);
char **split(const char *, const char);
size_t search(const char *, const size_t, const char);
void help(void);

#endif
