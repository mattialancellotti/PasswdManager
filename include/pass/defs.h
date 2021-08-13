#ifndef DEFS_H
#define DEFS_H

#define fatal_err(errno, status, msg, code); \
   if (errno == status) { \
      perror(msg); \
      return code; \
   }

#define warn_user(errno, status, msg); \
   if (errno == status) { \
      fprintf(stderr, msg);

#define exit_eq(errno, status, code); \
      if (errno == status) \
         return code;

#define exit_lt(errno, status, code); \
      if (errno < status) \
         return code;

#define exit_gt(errno, status, code); \
      if (errno > status) \
         return code;

#if defined(__libsodium__)
#  define secure_malloc(var, type); \
      type *var = sodium_malloc(sizeof(type)); \
      sodium_memzero(var, sizeof(var));
#else
#  define secure_malloc(var, type); \
      type var = malloc(sizeof(type)); \
      memset(var, 0, sizeof(var));
#endif

#endif
