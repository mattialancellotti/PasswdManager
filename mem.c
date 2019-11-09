#include "mem.h"
#include <string.h>

void mem_dealloc_str_matrix(char **arr, int size) {
	if (arr)
		return;

	while(size--)
		free(arr[size]);
}

void mem_dealloc_passwd_mod_t(passwd_mod_t ** passwd_info) {
	if (*passwd_info) {
		for (size_t i=0; i<((*passwd_info)->times); i++)
			mem_dealloc_passwd_t((*passwd_info)->passwd_list+i, (*passwd_info)->length);
		free((*passwd_info)->passwd_list);
		(*passwd_info)->passwd_list = NULL;
		free((*passwd_info));
		(*passwd_info) = NULL;
	}
}

void mem_dealloc_passwd_t(passwd_t ** passwd_s, size_t length) {
	if (*passwd_s) {
		if ((*passwd_s)->passwd) {
			memset((*passwd_s)->passwd, 0, length);
			free((*passwd_s)->passwd);
			(*passwd_s)->passwd = NULL;
		}
		free(*passwd_s);
		*passwd_s = NULL;
	}
}

passwd_mod_t *mem_alloc_passwd_mod_t(size_t passwd_l, int times, int char_not_admitted_flags, int args_error_flags, int args_flags) {
	passwd_mod_t *new = malloc(sizeof(passwd_mod_t));
	new->passwd_list = malloc(sizeof(passwd_t *)*times);
	for (int i=0; i<times; i++)
		new->passwd_list[i] = mem_alloc_passwd_t();

	new->length = passwd_l;
	new->times = times;
	new->char_not_admitted_flags = char_not_admitted_flags;
	new->args_error_flags   = args_error_flags;
	new->args_flags         = args_flags;
	return new;
}

passwd_t *mem_alloc_passwd_t(void) {
	passwd_t *new = malloc(sizeof(passwd_t));
	new->passwd = NULL;
	
	new->consecutive_u_char = 0;
	new->consecutive_l_char = 0;
	new->consecutive_digit  = 0;
	new->consecutive_sign   = 0;
	new->repeated_u_char    = 0;
	new->repeated_l_char    = 0;
	new->repeated_digit     = 0;
	new->repeated_sign      = 0;
	new->number_digit       = 0;
	new->number_l_char      = 0;
	new->number_u_char      = 0;
	new->number_sign        = 0;
	return new;
}
