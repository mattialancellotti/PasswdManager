#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "json.h"
#include "utils.h"

JSON *json_put(char *key, void *value, enum KIND kind) {
	JSON *new_json_object = malloc(sizeof(JSON));
	new_json_object->json_name = key;
	new_json_object->json_value = value;
	new_json_object->kind = kind;
	return new_json_object;
}

JSONObject *json_object_add(JSON *json, JSONObject *json_object) {
	if (!json_object) {
		json_object = malloc(sizeof(JSONObject));
		json_object->size = 2;
		json_object->current_index = 0;
		json_object->json_arr = malloc(sizeof(JSON)*json_object->size);
	}

	if (json_object->current_index == json_object->size) {
		json_object->size++;
		json_object = realloc(json_object, json_object->size);
	}
	json_object->json_arr[json_object->current_index++] = *json;
	return json_object;
}

JSONArray *json_array_add(JSONObject *json_object, JSONArray *json_array) {
	if (!json_array) {
		json_array = malloc(sizeof(JSONArray));
		json_array->size = 2;
		json_array->current_index = 0;
		json_array->json_obj_arr = malloc(sizeof(JSONObject)*json_array->size);
	}

	if (json_array->current_index == json_array->size) {
		json_array->size++;
		json_array = realloc(json_array, json_array->size);
	}
	json_array->json_obj_arr[json_array->current_index++] = *json_object;
	return json_array;
}

#ifdef DEV_MODE
	#define PRINT_JSON(x, y) printf("Creating json key: %ld - %s\n", x, y);
	#define PRINT_OBJ(x, y)  printf("Creating json object: %ld - %s\n", x, y);
	#define PRINT_ARR(x, y)  printf("Creating json array: %ld - %s\n", size, json_arr_str);
#endif
char *json_parser(JSON *json) {
	if (!json)
		return NULL;

	char *json_str = NULL, *str = NULL;
	size_t size = strlen(json->json_name)+4;
	if (json->kind != STRING) {
		if (json->kind == J_NULL) {
			str = malloc(sizeof(char)*5);
			str = "null\0";
		} else if (json->kind == NUMBER) {
			str = malloc(sizeof(char)*count_digits(*(float *)json->json_value)+20);
			sprintf(str, "%.3f", *(float *)json->json_value);
		} else if (json->kind == JSON_OBJ)
			str = json_obj_parser(json->json_value);
		else if (json->kind == JSON_ARR)
			str = json_arr_parser(json->json_value);
		else if (json->kind == BOOL) {
			if (*((int *)json->json_value)) {
				str = malloc(sizeof(char)*5);
				str = "true\0";
			} else {
				str = malloc(sizeof(char)*6);
				str = "false\0";
			}
		} else
			return NULL;
		size += strlen(str);
	} else {
		str = json->json_value;
		size += strlen(str) + 2;
	}

	json_str = malloc(sizeof(char)*size);

	//creating value
	strcpy(json_str, "\"");
	strcat(json_str, json->json_name);
	strcat(json_str, "\":");

	if (json->kind == STRING) {
		strcat(json_str, "\"");
		strcat(json_str, str);
		strcat(json_str, "\"");
	} else
		strcat(json_str, str);

	strcat(json_str, "\0");
	PRINT_JSON(size, json_str)
	return json_str;
}

char *json_obj_parser(JSONObject *json_object) {
	char *json_obj_str = malloc(sizeof(char)*STD_JSON_OBJ_SIZE);
	char *tmp = NULL;
	size_t size = 3, times=1;
	strcat(json_obj_str, "{");
	for (size_t i=0; i<json_object->current_index; i++) {
		if (!(tmp = json_parser(json_object->json_arr+i)))
			return NULL;
		size += strlen(tmp);

		if (i != json_object->current_index-1)
			size++;

		//checking size
		if (size >= STD_JSON_OBJ_SIZE*times) {
			times++;
			json_obj_str = realloc(json_obj_str, STD_JSON_OBJ_SIZE*times);
		}

		strcat(json_obj_str, tmp);
		if (i != json_object->current_index-1)
			strcat(json_obj_str, ",");
	}

	strcat(json_obj_str, "}\0");
	json_obj_str = realloc(json_obj_str, size);
	PRINT_OBJ(size, json_obj_str)
	printf("Creating json object: %ld - %s\n", size, json_obj_str);
	return json_obj_str;
}

char *json_arr_parser(JSONArray *json_array) {
	char *json_arr_str = malloc(sizeof(char)*STD_JSON_OBJ_SIZE);
	char *tmp = NULL;
	size_t size = 3, times=1;
	strcat(json_arr_str, "[");
	for (size_t i=0; i<json_array->current_index; i++) {
		if (!(tmp = json_obj_parser(json_array->json_obj_arr+i)))
			return NULL;
		size += strlen(tmp);

		if (i != json_array->current_index-1)
			size++;

		if (size >= STD_JSON_OBJ_SIZE*times) {
			times++;
			json_arr_str = realloc(json_arr_str, STD_JSON_OBJ_SIZE*times);
		}

		strcat(json_arr_str, tmp);
		if (i != json_array->current_index-1)
			strcat(json_arr_str, ",");
	}
	strcat(json_arr_str, "]\0");
	json_arr_str = realloc(json_arr_str, size);
	PRINT_ARR(size, json_arr_str)
	return json_arr_str;
}
