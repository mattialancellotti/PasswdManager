#include "mem.h"
#include "args.h"
#include "utils.h"

//args_error_flags
//...0 [ COMING_SOON_ARG] 0 [ UNKNOWN_ARG ] 0 [ PARAMETER_ERROR ] 0 [ PARAMETER_MISSING ] 
//
//args_flags
//...0 [ version action ] 0 [ help action ] 0 [ print action ] 0 [ save action ]
passwd_mod_t *handle_args(const char **argv, const int argc) {
	size_t length = DEFAULT_PASSWD_SIZE, times = 1;
	int char_not_admitted_flags = 0, args_error_flags = 0, args_flags = 0;

	for (int i=1; i<argc; i++)
		if (argv[i][0] == '-') {
			if (strlen(argv[i]+1) > 1) {
				if (!strcmp(argv[i]+1, "length")) {
					if ((argc-1) == i)
						args_error_flags|=PARAMETER_MISSING;
					else if (check_num(argv[++i]))
						length = (size_t)atoi(argv[i]);
					else
						args_error_flags|=PARAMETER_ERROR;
				} else if (!strcmp(argv[i]+1, "not-admitted")) {
					if ((argc-1) == i)
						args_error_flags|=PARAMETER_MISSING;
					else if (argv[++i][0] != '-') {
					      char_not_admitted_flags = get_flags(argv[i]);
					      if (char_not_admitted_flags&16)
					      		args_error_flags|=PARAMETER_ERROR;
					} else
						args_error_flags|=PARAMETER_ERROR;
				} else if (!strcmp(argv[i]+1, "times")) {
					if ((argc-1) == i)
						args_error_flags|=PARAMETER_MISSING;
					else if (check_num(argv[++i]))
						times = atoi(argv[i]);
					else
						args_error_flags|=PARAMETER_ERROR;
				} else if (!strcmp(argv[i]+1, "save"))
					args_error_flags|=COMING_SOON_ARG;
				else if (!strcmp(argv[i]+1, "use"))
					args_error_flags|=COMING_SOON_ARG;
				else if (!strcmp(argv[i]+1, "print"))
					args_flags|=2;
				else if (!strcmp(argv[i]+1, "help"))
					args_flags|=4;
				else if (!strcmp(argv[i]+1, "version"))
					args_flags|=8;
				else
					args_error_flags|=UNKNOWN_ARG;
			} else if (strlen(argv[i]+1) == 1) {
				switch(argv[i][1]) {
				case 'l':
					if ((argc-1) == i)
						args_error_flags|=PARAMETER_MISSING;
					else if (check_num(argv[++i]))
						length = (size_t)atoi(argv[i]);
					else
						args_error_flags|=PARAMETER_ERROR;
					break;
				case 'n':
					if ((argc-1) == i)
						args_error_flags|=PARAMETER_MISSING;
					else if (argv[++i][0] != '-') {
						char_not_admitted_flags = get_flags(argv[i]);
						if (char_not_admitted_flags&16)
							args_error_flags|=PARAMETER_ERROR;
					} else
						args_error_flags|=PARAMETER_ERROR;
					break;
				case 't':
					if ((argc-1) == i)
						args_error_flags|=PARAMETER_MISSING;
					else if (check_num(argv[++i]))
						times = atoi(argv[i]);
					else
						args_error_flags|=PARAMETER_ERROR;
					break;
				case 's':
					args_error_flags|=COMING_SOON_ARG;
					break;
				case 'p':
					args_flags|=2;
					break;
				case 'h':
					args_flags|=4;
					break;
				case 'u':
					args_error_flags|=COMING_SOON_ARG;
					break;
				case 'v':
					args_flags|=8;
					break;
				default:
					args_error_flags=UNKNOWN_ARG;
				}
			} else
				args_error_flags|=UNKNOWN_ARG;
		} else
			args_error_flags|=UNKNOWN_ARG;

	return mem_alloc_passwd_mod_t(length, times, char_not_admitted_flags, args_error_flags, args_flags);
}

//check the input of -not-admitted
unsigned get_flags(const char *str) {
	int i=0, flags=0;  // 0 ->digit, 0->l_char, 0->u_char, 0->sign
	char **arr_not_admitted = split(str, ',');
	while(i<MAX_NOT_ADMITTED_STR_NUM && arr_not_admitted[i]) {
		if (!strcmp(arr_not_admitted[i], "digit"))
			flags|=8;
		else if (!strcmp(arr_not_admitted[i], "l_char"))
			flags|=4;
		else if (!strcmp(arr_not_admitted[i], "u_char"))
			flags|=2;
		else if (!strcmp(arr_not_admitted[i], "sign"))
			flags|=1;
		else {
			flags|=16;
			break;
		}
		i++;
	}

	mem_dealloc_str_matrix(arr_not_admitted, i-1);
	return flags;
}			

#define PRINT_E(x, y) if (args_error_flags&x)\
				printf("%s["#x"]%s: "y".\n", RED, RESET);
void print_args_error(const size_t args_error_flags) {
	PRINT_E(UNKNOWN_ARG, "unknown argument detected")
	PRINT_E(PARAMETER_MISSING, "this argument requires a parameter")
	PRINT_E(PARAMETER_ERROR, "this argument requires a different kind of parameter")
	PRINT_E(COMING_SOON_ARG, "this argument is not defined yet");
}
