#include "utils.h"

//check if it's a number
unsigned check_num(const char *str) {
	int i=0;
	while(str[i])
		if (!(isdigit(str[i++])))
			return 0;
	return 1;
}

size_t search(const char * arr, const size_t size, const char c) {
	size_t i=0;
	while(i<size)
		if (c == arr[i++])
			return i;
	return 0;
}

void help(void) {
	printf("Passwd Generator\n");
	printf("\t-l, -length: to specify the length of the password.\n");
	printf("\t-n, -not-admitted: to specify which type of character is not allowed [digit - u_char - l_char - sign] (usage: -not-admitted digit,u_char)\n");
	printf("\t-t, -times: to specify how many password you need\n");
	printf("\t-p, -print: to print the stats\n");
	printf("\t-h, -help: to show this message\n");
	printf("\t-v, -version: to print the current version of the program\n");
}

//split function to split a string
char **split(const char *str, const char c) {
	char **arr_not_admitted = malloc(sizeof(char *)*MAX_NOT_ADMITTED_STR_NUM);
	char tmp[255] = { 0 };
	int i=0, j=0, arr_i=0;
	while(str[i]) {
		if (str[i] == c) {
			tmp[j] = '\0';
			arr_not_admitted[arr_i] = malloc(sizeof(char)*j);
			strcpy(arr_not_admitted[arr_i++], tmp);
			j=0;
		} else
			tmp[j++] = str[i];
		i++;
	}
	if (str[i-1] != c) {
		tmp[j] = '\0';
		arr_not_admitted[arr_i] = malloc(sizeof(char)*j);
		strcpy(arr_not_admitted[arr_i++], tmp);
	}

	if (arr_i < MAX_NOT_ADMITTED_STR_NUM-1) //if there are less then 4 not admitted sign, then create a stop-signal
		arr_not_admitted[arr_i] = NULL;

	return realloc(arr_not_admitted, arr_i);
}

size_t count_digits(float num) {
	float second_part = num - (int)num;
	size_t counter = 0, dec_digits=3;

  if (!num)
    return 1;
	//counting first part of digits
	counter = log10((int)num)+1;
  if (!second_part)
    return counter;

	while(dec_digits--)
		second_part*=10;

	//approximation
	if (((second_part - (int)second_part)*10) > 4)
		second_part+=1;
	counter += log10((int)second_part);
	
	return counter;
}
