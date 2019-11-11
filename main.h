#ifndef MAIN_H
#define MAIN_H

#include<stdlib.h>
#include<stdio.h>

#define RED "\x1b[31m"
#define YELLOW "\x1b[33m"
#define RESET "\x1b[0m"

#define VERSION 2.5
#define MAJOR_RELEASE_DATE "2019-11-04"
#define MINOR_RELEASE_DATE  "2019-11-09"
#define DEFAULT_PASSWD_SIZE 8

#define JSON_SIZE_TIME 5
typedef struct {
	char * restrict passwd;
	
	int consecutive_u_char, consecutive_l_char, consecutive_digit, consecutive_sign;
	int repeated_u_char, repeated_l_char, repeated_digit, repeated_sign;
	int number_digit, number_l_char, number_u_char, number_sign;
} passwd_t;

typedef struct {
	passwd_t ** passwd_list;
	char *module_name;

	size_t length, times;
	int char_not_admitted_flags, args_error_flags, args_flags;
	//args_error_flags saves which errors occurred
	//args_flags choices made by the user
} passwd_mod_t;

typedef struct JSON JSON;
typedef struct JSONObject JSONObject;
typedef struct JSONArray JSONArray;

struct JSONObject {
	JSON *json_arr;
	size_t size, current_index;
};

struct JSON {
	char *json_name;
	void *json_value;
};

struct JSONArray {
	JSONObject *json_obj_arr;
	size_t size, current_index;
};

char *create_passwd(const size_t, const int);
void check_passwd(passwd_t ** const, const size_t);
void print(const passwd_t * const);
void save(FILE *, passwd_mod_t ** const);

JSON *json_put(char *, void *);
JSONObject *json_object_add(JSON *, JSONObject *);
JSONArray *json_array_add(JSONObject *, JSONArray *);
void json_str_parser(JSONObject *);
#endif
