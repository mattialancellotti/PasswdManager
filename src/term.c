#define _GNU_SOURCE

#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <pass/term.h>
#include <pass/os.h>

char *ask_pass(void)
{
   /* 
    * Declaring local struct to handle terminal's opts.
    * Look at `man termios(3)`
    */
   struct termios term;

   /* Getting the opts */
   if (tcgetattr(0, &term) == -1)
      perror("tcgetattr");

   /* Disabling echo */
   term.c_lflag ^= ECHO;
   if (tcsetattr(0, TCSANOW, &term) == -1)
      perror("tcsetattr");

   /* Termporary solution, I know this is bad */
   char *str = NULL;
   size_t default_len = 20;
   size_t len = getline(&str, &default_len, stdin);
   fflush(stdin);

   /* Enabling echo */
   term.c_lflag ^= ECHO;
   if (tcsetattr(0, TCSANOW, &term) == -1)
      perror("tcsetattr");

   return str;
}

#define case_ins(var, ch) ((var == ch) || (var == (ch+32)))
char ask_confirmation(const char *msg)
{
   const char *default_msg = "Are you sure? (y/N) >>";
   const char short_yes = 'y', short_no = 'n';
   char *answer = NULL;
   char ret = '\0';

   do {
      printf("%s", ( msg == NULL ? default_msg : msg));
      answer = users_input();

      /* If the users is trying the be smart, outsmart him */
      if (case_ins(answer[0], 'Y') || case_ins(answer[0], 'N'))
         ret = tolower(answer[0]);
      else
         fprintf(stderr, "'%s', what's that?", answer);

      free(answer);
   } while (ret == '\0');

   /* Returing successfully */
   return ret;
}
