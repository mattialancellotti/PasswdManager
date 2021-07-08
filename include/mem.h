#ifndef MEM_H
#define MEM_H

#include <main.h>

void mem_dealloc_passwd_t(passwd_t ** /*passwd_s*/, const size_t /*lenght*/);

/* It safely allocates and initialize a password type */
passwd_t *mem_alloc_passwd_t(void);

#endif
