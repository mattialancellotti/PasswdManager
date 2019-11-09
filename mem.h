#ifndef MEM_H
#define MEM_H

#include "main.h"

void mem_dealloc_str_matrix(char **, int);
void mem_dealloc_passwd_mod_t(passwd_mod_t **); // safe free function for the passwd_mod_t struct
void mem_dealloc_passwd_t(passwd_t **, size_t);
passwd_mod_t *mem_alloc_passwd_mod_t(size_t, int, int, int, int);
passwd_t *mem_alloc_passwd_t(void);

#endif
