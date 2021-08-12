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

#define just_exit(errno, status, code); \
      if (errno == status) \
         return code; \

#endif
