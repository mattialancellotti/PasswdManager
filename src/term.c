#include <termios.h>
#include <unistd.h>
#include <stdio.h>

#include <pass/term.h>

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
   scanf("%m[a-z]", &str);
   /* Enabling echo */
   term.c_lflag ^= ECHO;
   if (tcsetattr(0, TCSANOW, &term) == -1)
      perror("tcsetattr");

   return str;
}
