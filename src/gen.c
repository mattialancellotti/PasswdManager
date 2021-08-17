#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

#include <pass/main.h>
#include <pass/gen.h>

/*
 * This function creates a password of length `length`. Also  it accepts a
 * `flags` that indicates which of { 'u_char', 'l_char' ... } are not accepted
 */
static char *create_passwd(const size_t /*length*/, const int /*flags*/);

/* TODO:
 *  - Parameter checking;
 *  - Secure malloc;
 *  - Lock memory;
 */
char **passwd_generator(passwd_conf_t *passwd_conf)
{
   size_t passwd_length = passwd_conf->length;
   size_t passwd_times  = passwd_conf->times;
   unsigned denied_char = passwd_conf->char_not_admitted;

   char **passwd_list = malloc(sizeof(char *) * passwd_times);

   for (size_t i=0; i<passwd_times; i++) {
      passwd_list[i] = create_passwd(passwd_length, denied_char);

      /* Wait unless this is the last password */
      if (passwd_times != i+1)
         /*
          * This is needed to create new seeds, without the sleep function all
          * the passwords generated are going to be the same.
          */
	 sleep(1);
   }

   return passwd_list;
}

/* Really basic way of creating a password */
static char *create_passwd(const size_t length, const int flags)
{
   /* TODO:
    *  - secure_malloc;
    *  - lock memory;
    */
   char *passwd = malloc(sizeof(char)*length+1);
   size_t i=0;
   srand(time(NULL));
   
   while(i<length) {
      passwd[i] = rand()%94+33;
      /* This if checks if the picked character is accepted or not */
      if ((isdigit(passwd[i]) && flags&8) || (islower(passwd[i]) && flags&4)
					  || (isupper(passwd[i]) && flags&2)
					  || (ispunct(passwd[i]) && flags&1))
         continue;
      
      i++;
   }

  passwd[i] = '\0';

  return passwd;
}
