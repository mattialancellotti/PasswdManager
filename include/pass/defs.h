#ifndef DEFS_H
#define DEFS_H

#define PROG_ROOT ".local/share/"
#define ROOT_PATH "ezpass/"
#define PASS_HASH "passwd"
#define PASS_DB   "db/"

#define INIT 00001000
#define STAT 00000100
#define HELP 00000010
#define VERS 00000001

#define emptyness(bitarr, bitcount) ((bitarr >> bitcount) == 0)
#define check_bit(bitarr, bitpos) (bitarr & bitpos)
#define set_bit(bitarr, bitpos) (bitarr |= bitpos)

#define ifdef_free(var); \
   if (var != NULL) { \
      free(var);  \
      var = NULL; \
   }

#define system_err(condition, msg, exit_code); \
   if (condition) { \
      perror(msg); \
      return exit_code; \
   }

#define prog_err(condition, msg, action); \
   if (condition) { \
      fprintf(stderr, "(%s) %d: %s: %s\n", __FILE__, __LINE__, __func__, msg); \
      action; \
   }

/* TODO: __DEPRECATED__ */
#define fatal_err(errno, status, msg, code); \
   if (errno == status) { \
      perror(msg); \
      return code; \
   }

#define warn_user(condition, msg); \
   if (condition) \
      fprintf(stderr, msg);

/* Use the same do_if/exit_if as the BrainFuck Interpreter */
#define exit_if(condition, code); \
   if (condition) \
      return code;

      
/* TODO: __DEPRECATED__ */
#define exit_eq(errno, status, code); \
      if (errno == status) \
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
