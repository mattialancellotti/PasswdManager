#ifndef DEFS_H
#define DEFS_H

#define ifdef_free(var); \
   if (var != NULL) { \
      free(var);  \
      var = NULL; \
   }

/*
 * TODO:
 *  - system_err(condition, msg, exit_code);
 *    To safely inform the user that something out og our reach happened.
 *  - prog_err(condition, msg, action);
 *    To safely inform the user that an error is being taken care of.
 */
#define fatal_err(errno, status, msg, code); \
   if (errno == status) { \
      perror(msg); \
      return code; \
   }

#define warn_user(condition, msg); \
   if (condition) \
      fprintf(stderr, msg);

#define exec_if(condition, action); \
   if (condition) { \
      action; \
   }

/* Use the same do_if/exit_if as the BrainFuck Interpreter */
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
