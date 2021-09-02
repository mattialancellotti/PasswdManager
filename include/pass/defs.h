#ifndef DEFS_H
#define DEFS_H

#define PROG_ROOT ".local/share/"
#define ROOT_PATH "ezpass/"
#define PASS_HASH "passwd"
#define PASS_DB   "db/"

extern int behaviour;
extern char *db_path;

#define INIT 00001111
#define VERB 00000100
#define STAT 00000010
#define ASK  00000001

#define HELP 01000000
#define VERS 00100000
#define GENE 00010000

#define check_flag(flag) (flag)
#define strict_check(bitarr, bitpos) ((bitarr & bitpos) == bitpos)
#define check_bit(bitarr, bitpos) (bitarr & bitpos)
#define set_bit(bitarr, bitpos) (bitarr |= bitpos)
#define reset_bit(bitarr, bitpos) (bitarr = bitpos)

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

#define warn_user(condition, msg, exit_code); \
   if (condition) { \
      fprintf(stderr, "%s\n", msg); \
      return exit_code; \
   }

/* Use the same do_if/exit_if as the BrainFuck Interpreter */
#define exit_if(condition, code); \
   if (condition) \
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
