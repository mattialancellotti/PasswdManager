#include <sodium.h>
#include <string.h>

#include "mem.h"

/* Safely deallocates a password */
void mem_dealloc_passwd_t(passwd_t ** passwd_s, const size_t length)
{
   /* Some controls to avoid segmatation faults */
   if (*passwd_s) {
      if ((*passwd_s)->passwd) {
         /* 
          * Clears out the password to avoid it being stolen even after the
          * deallocation.
          */
	 memset((*passwd_s)->passwd, 0, length);
	 free((*passwd_s)->passwd);
	 (*passwd_s)->passwd = NULL;
      }

      free(*passwd_s);
      *passwd_s = NULL;
   }
}

passwd_t *mem_alloc_passwd_t(void)
{
   /* Allocates and initialize the struct `passwd_t`, use sizeof(*new) understand */
#if defined(__libsodium__)
   passwd_t *new = sodium_malloc(sizeof(passwd_t));
   sodium_memzero(new, 56);
#else
   passwd_t *new = malloc(sizeof(passwd_t));
   memset(new, 0, 56);
#endif

   return new;
}
