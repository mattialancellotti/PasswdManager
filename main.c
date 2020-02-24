#include<unistd.h>
#include<time.h>

#include "utils.h"
#include "args.h"
#include "mem.h"

int main(const int argc, const char **argv) {
	passwd_mod_t *passwd_info = handle_args(argv, argc);

	if (passwd_info->args_error_flags) {
		print_args_error(passwd_info->args_error_flags);
		goto exit;
	}

	if (passwd_info->args_flags&8)
		printf("Password generator:\n\t| Current version: %.1f\n\t| Release date: %s\n\t| Last update date: %s\n", VERSION, MAJOR_RELEASE_DATE, MINOR_RELEASE_DATE);

	if (passwd_info->args_flags&4)
		help();

	for (size_t i=0; i<passwd_info->times; i++) {
		passwd_info->passwd_list[i]->passwd = create_passwd(passwd_info->length, passwd_info->char_not_admitted_flags);
		check_passwd(passwd_info->passwd_list+i, passwd_info->length);
		printf("Password: %s%s%s\n", YELLOW, passwd_info->passwd_list[i]->passwd, RESET);

		if (passwd_info->args_flags&2)
			print(passwd_info->passwd_list[i]);

		if (passwd_info->times != i+1)
			sleep(1);
	}
exit:
	mem_dealloc_passwd_mod_t(&passwd_info);
	return 0;
}

char *create_passwd(const size_t length, const int flags) {
	char *passwd = malloc(sizeof(char)*length+1);
	size_t i=0;
	srand(time(NULL));
	while(i<length) {
		passwd[i] = rand()%94+33;
		if ((isdigit(passwd[i]) && flags&8)
					|| (islower(passwd[i]) && flags&4)
					|| (isupper(passwd[i]) && flags&2)
					|| (ispunct(passwd[i]) && flags&1))
			continue;
		i++;
	}

  passwd[i] = '\0';

  return passwd;
}

void check_passwd(passwd_t ** const passwd_created, const size_t length) {
	if (!(*passwd_created) && !((*passwd_created)->passwd))
		return;

	enum TYPE last_c = NONE;
	char already_used[length];
	size_t arr_i=0, res=0;
	char tmp;

	for (size_t i=0; i<length; i++) {
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

void print(const passwd_t * const passwd_info) {
	printf("\n%s===CONSECUTIVE CHARACTERS===%s\n", RED, RESET);
	printf("\t| Consecutive upper case chars: %d\n\t| Consecutive lower case chars: %d\n\t| Consecutive digits: %d\n\t| Consecutive signs: %d\n", passwd_info->consecutive_u_char, passwd_info->consecutive_l_char, passwd_info->consecutive_digit, passwd_info->consecutive_sign);
	printf("\n%s===REPEATED CHARACTERS===%s\n", RED, RESET);
	printf("\t| Repeated upper case chars: %d\n\t| Repeated lower case chars: %d\n\t| Repeated digits: %d\n\t| Repeated signs: %d\n", passwd_info->repeated_u_char, passwd_info->repeated_l_char, passwd_info->repeated_digit, passwd_info->repeated_sign);
	printf("\n%s===NUMBER OF EACH TYPE===%s\n", RED, RESET);
	printf("\t| Number of upper case chars: %d\n\t| Number of lower case chars: %d\n\t| Number of digits: %d\n\t| Number of signs: %d\n", passwd_info->number_u_char, passwd_info->number_l_char, passwd_info->number_digit, passwd_info->number_sign);
}
