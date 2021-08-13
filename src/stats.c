#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <pass/stats.h>
#include <pass/defs.h>

/* TODO:
 *  - Create a find_enum function to find the correct enum var for a variable;
 *  - Create a function to calculate how safe is a password;
 *  - Change consecutive_chars to a hash table (hash_table[char]++);
 */
void check_passwd(passwd_t ** passwd_created, const size_t length) {
   /* Checking the parameters */
   exit_eq(passwd_created, NULL, );
   exit_eq(*passwd_created, NULL, );
   exit_eq((*passwd_created)->passwd, NULL, );

   size_t pwlen = strlen((*passwd_created)->passwd);
   enum TYPE last_c = NONE;
   char already_used[pwlen];
   size_t arr_i=0, res=0;
   char tmp;

   for (size_t i=0; i<pwlen; i++) {
      if (!(res = search(already_used, arr_i, (tmp=(*passwd_created)->passwd[i]))))
         already_used[++arr_i] = tmp;

      if (isdigit(tmp)) {
	 if (last_c == DIGIT)
	    (*passwd_created)->consecutive_digit++;
	 else
	    last_c = DIGIT;

         (*passwd_created)->number_digit++;

	 if (res)
	    (*passwd_created)->repeated_digit++;
      } else if (islower(tmp)) {
	 if (last_c == L_CHAR)
	    (*passwd_created)->consecutive_l_char++;
	 else
	    last_c = L_CHAR;

	 (*passwd_created)->number_l_char++;

	 if (res)
	    (*passwd_created)->repeated_l_char++;
      } else if (isupper(tmp)) {
	 if (last_c == U_CHAR)
	    (*passwd_created)->consecutive_u_char++;
	 else
	    last_c = U_CHAR;

	 (*passwd_created)->number_u_char++;

	 if (res)
	    (*passwd_created)->repeated_u_char++;
      } else {
	 if (last_c == SIGN)
	    (*passwd_created)->consecutive_sign++;
	 else
	    last_c = SIGN;

	 (*passwd_created)->number_sign++;

	 if (res)
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

size_t search(const char * arr, const size_t size, const char c)
{
   size_t i=0;
   while(i<size)
      if (c == arr[i++])
	 return i;
   return 0;
}
