#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <pass/stats.h>
#include <pass/defs.h>

static passwd_t *passwd_malloc_t(const char* /*passwd*/);
static void passwd_free(passwd_t* /*passwd_service*/);

void passwd_stats(const char *passwd)
{
   passwd_t *stats = passwd_malloc_t(passwd);
   check_passwd(&stats);

   /* Printing everything out */
   print(stats);
   passwd_free(stats);
}

/* TODO:
 *  - Create a find_enum function to find the correct enum var for a variable;
 *  - Create a function to calculate how safe is a password;
 */
#define type_of(check_var, var_type, inc_var) \
   if (check_var == var_type) \
      inc_var++; \
   else \
      check_var = var_type;

void check_passwd(passwd_t **passwd_created) {
   /* Checking the parameters */
   exit_if(!passwd_created, );
   exit_if(!(*passwd_created), );

   size_t pwlen = strlen((*passwd_created)->passwd);
   enum TYPE last_c = NONE;
   char already_used[pwlen];
   size_t arr_i=0;
   int res=0;
   char tmp;

   for (size_t i=0; i<pwlen; i++) {
      if ((res = search(already_used, arr_i, (tmp=(*passwd_created)->passwd[i]))) == -1)
         already_used[arr_i++] = tmp;

      if (isdigit(tmp)) {
         type_of(last_c, DIGIT, (*passwd_created)->consecutive_digit);
         (*passwd_created)->number_digit++;

	 if (res != -1)
	    (*passwd_created)->repeated_digit++;
      } else if (islower(tmp)) {
         type_of(last_c, L_CHAR, (*passwd_created)->consecutive_l_char);
	 (*passwd_created)->number_l_char++;

	 if (res != -1)
	    (*passwd_created)->repeated_l_char++;
      } else if (isupper(tmp)) {
         type_of(last_c, L_CHAR, (*passwd_created)->consecutive_u_char);
	 (*passwd_created)->number_u_char++;

	 if (res != -1)
	    (*passwd_created)->repeated_u_char++;
      } else {
         type_of(last_c, L_CHAR, (*passwd_created)->consecutive_sign);
	 (*passwd_created)->number_sign++;

	 if (res != -1)
	    (*passwd_created)->repeated_sign++;
      }
   }
}

/* You really have to make this look better */
void print(const passwd_t *passwd_info) {
	printf("\n%s===CONSECUTIVE CHARACTERS===%s\n", RED, RESET);
	printf("\t| Consecutive upper case chars: %d\n\t| Consecutive lower case chars: %d\n\t| Consecutive digits: %d\n\t| Consecutive signs: %d\n", passwd_info->consecutive_u_char, passwd_info->consecutive_l_char, passwd_info->consecutive_digit, passwd_info->consecutive_sign);
	printf("\n%s===REPEATED CHARACTERS===%s\n", RED, RESET);
	printf("\t| Repeated upper case chars: %d\n\t| Repeated lower case chars: %d\n\t| Repeated digits: %d\n\t| Repeated signs: %d\n", passwd_info->repeated_u_char, passwd_info->repeated_l_char, passwd_info->repeated_digit, passwd_info->repeated_sign);
	printf("\n%s===NUMBER OF EACH TYPE===%s\n", RED, RESET);
	printf("\t| Number of upper case chars: %d\n\t| Number of lower case chars: %d\n\t| Number of digits: %d\n\t| Number of signs: %d\n", passwd_info->number_u_char, passwd_info->number_l_char, passwd_info->number_digit, passwd_info->number_sign);
}

int search(const char * arr, size_t size, const char c)
{
   int i=0;
   while(size--) {
      if (c == arr[i++])
	 return i;
   }

   return -1;
}

static passwd_t *passwd_malloc_t(const char *passwd)
{
   passwd_t *new = malloc(sizeof(passwd_t));

   memset(new, 0, sizeof(passwd_t));
   new->passwd = passwd;

   return new;
}

static void passwd_free(passwd_t *passwd_service)
{
   if (passwd_service) {
      free(passwd_service);
      passwd_service = NULL;
   }
}
