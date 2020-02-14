#include <string.h>
#include <stdlib.h>
#include <stdio.h> 

#include "json.h"
#include "utils.h"

//Garbage collector used to determine if the allocated pointer is addressed on the heap or not so we can free it
static void **json_allocated_addresses = NULL;
static int *json_allocated_addresses_type = NULL;
static size_t current_free_pos = 0, times_alloc = 0;

JSONObject *first_obj = NULL;

void addrs_arr_alloc(void) {
	times_alloc++;
 	json_allocated_addresses = realloc(json_allocated_addresses, times_alloc*ALLOCATED_SPACE);
 	json_allocated_addresses_type = realloc(json_allocated_addresses_type, times_alloc*ALLOCATED_SPACE);

 	PRINT_INIT(times_alloc*ALLOCATED_SPACE)
}

void *addrs_alloc(size_t size, enum MEM_KIND kind) {
	if (current_free_pos == (times_alloc*ALLOCATED_SPACE))
		addrs_arr_alloc();

	json_allocated_addresses[current_free_pos] = malloc(size);
	json_allocated_addresses_type[current_free_pos] = kind;

	PRINT_ADDRS_ALLOC(current_free_pos, size, kind, (void *)json_allocated_addresses[current_free_pos])
	return json_allocated_addresses[current_free_pos++];
}

void *addrs_realloc(void *ptr, const size_t size) {
	int pos = addrs_search(ptr);
	if (pos == -1)
		return NULL;

	ptr = realloc(ptr, size);
	json_allocated_addresses[pos] = ptr;

	PRINT_ADDRS_REALLOC(pos, (void *)json_allocated_addresses[pos]);
	return ptr;
}

int addrs_search(const void *ptr) {
	for (size_t i=0; i<current_free_pos; i++)
		if (ptr == json_allocated_addresses[i])
			return i;

	return -1;
}

JSON *json_put(char *key, void *value, enum KIND n_kind) {
	JSON *new_json_object = addrs_alloc(sizeof(JSON), JSON_KEY);

	new_json_object->json_name = malloc(sizeof(char)*strlen(key)+1);
	memset(new_json_object->json_name, '\0', sizeof(char)*strlen(key)+1);
	strcpy(new_json_object->json_name, key);

	if (n_kind == STRING) {
		new_json_object->json_value = malloc(sizeof(char)*strlen((char *)value)+1);
		memset(new_json_object->json_value, '\0', sizeof(char)*strlen((char *)value)+1);
		strcpy(new_json_object->json_value, (char *)value);
	} else if (n_kind == NUMBER || n_kind == BOOL)
		new_json_object->json_value = value;
	else if (n_kind == JSON_OBJ) {
		new_json_object->json_value = value;
		first_obj = NULL;
	} else if (n_kind == JSON_ARR) {
		new_json_object->json_value = value;
		int pos = addrs_search(value);
		if (pos != -1)
			json_allocated_addresses[pos] = NULL;
	} else
		new_json_object->json_value = NULL;

	new_json_object->kind = n_kind;

	return new_json_object;
}

#define OBJ_DEF 2
JSONObject *json_object_add(JSON *json, JSONObject *json_object) {
	if (!json_object) {
		json_object = malloc(sizeof(JSONObject));
		json_object->size = OBJ_DEF;
		json_object->current_index = 0;
		json_object->json_arr = malloc(sizeof(JSON)*OBJ_DEF);
	}

	int pos = addrs_search(json);
	if (pos != -1)
		json_allocated_addresses[pos] = NULL;

	first_obj = json_object;

	if (json_object->current_index == json_object->size) {
		json_object->size+=OBJ_DEF;
		json_object->json_arr = realloc(json_object->json_arr, sizeof(JSON)*json_object->size);
	}

	json_object->json_arr[json_object->current_index++] = *json;
	return json_object;
}

JSONArray *json_array_add(JSONObject *json_object, JSONArray *json_array) {
	if (!json_array) {
		json_array = addrs_alloc(sizeof(JSONArray), JSON_ARR);
		json_array->size = OBJ_DEF;
		json_array->current_index = 0;
		json_array->json_obj_arr = malloc(sizeof(JSONObject)*OBJ_DEF);
	}

	first_obj = NULL;

	if (json_array->current_index == json_array->size) {
		json_array->size+=OBJ_DEF;
		json_array->json_obj_arr = addrs_realloc(json_array, sizeof(JSONObject)*json_array->size);
	}

	json_array->json_obj_arr[json_array->current_index++] = *json_object;
	return json_array;
}

char *json_parser(JSON *json) {
	if (!json)     return NULL;

	char *json_parse = NULL, *json_value = NULL;
	size_t size = 4+strlen(json->json_name);

	if (json->kind != STRING) {
		if (json->kind == J_NULL) {
			json_value = malloc(sizeof(char)*5);
			strcpy(json_value, "null\0");
		} else if (json->kind == JSON_OBJ) {
			json_value = json_obj_parser(json->json_value);
		} else if (json->kind == JSON_ARR) {
			json_value = json_arr_parser(json->json_value);
		} else if (json->kind == BOOL)
			if (*(int *)json->json_value) {
				json_value = malloc(sizeof(char)*5);
				strcpy(json_value, "true\0");
			} else {
				json_value = malloc(sizeof(char)*6);
				strcpy(json_value, "false\0");
			}
		else if (json->kind == NUMBER) {
			json_value = malloc(sizeof(char)*count_digits(*(float *)json->json_value)+2);
			sprintf((char *)json_value, "%.3f", *(float *)json->json_value);
		}
		size += strlen(json_value);
	} else {
		json_value = malloc(sizeof(char)*strlen(json->json_value)+1);
		strcpy(json_value, json->json_value);
		strcat(json_value, "\0");
		size += strlen((char *)json->json_value) + 2;
	}

	json_parse = malloc(sizeof(char)*size);

	//creating value
	strcpy(json_parse, "\"");
 	strcat(json_parse, json->json_name);
	strcat(json_parse, "\":");

	if (json->kind == STRING) {
		strcat(json_parse, "\"");
		strcat(json_parse, json_value);
		strcat(json_parse, "\"");
	} else
		strcat(json_parse, json_value);

	strcat(json_parse, "\0");

	free(json_value);
	json_value = NULL;

	PRINT_JSON(size, json_parse)
	return json_parse;
}

char *json_obj_parser(JSONObject *json_object) {
	size_t size = 3, tmp_size = STD_JSON_OBJ_SIZE;
	char *json_obj_str = malloc(sizeof(char)*tmp_size), *tmp = NULL;

	strcpy(json_obj_str, "{");

	for (size_t i=0; i<json_object->current_index; i++) {
		if (!(tmp = json_parser(json_object->json_arr+i)))
			goto json_obj_exit;

		size += strlen(tmp);

		if (i != json_object->current_index-1)
			size+=2;

		//checking size
		if (size >= tmp_size) {
			tmp_size = STD_JSON_OBJ_SIZE + size;
			json_obj_str = realloc(json_obj_str, sizeof(char)*tmp_size);
		}

		strcat(json_obj_str, tmp);
		free(tmp);

		if (i != json_object->current_index-1)
			strcat(json_obj_str, ",");
	}

json_obj_exit:
	strcat(json_obj_str, "}\0");
	json_obj_str = realloc(json_obj_str, sizeof(char)*size);

	PRINT_OBJ(size, json_obj_str)
	return json_obj_str;
}

char *json_arr_parser(JSONArray *json_array) {
	size_t size = 3, tmp_size = STD_JSON_OBJ_SIZE;
	char *json_arr_str = malloc(sizeof(char)*tmp_size), *tmp = NULL;

	strcpy(json_arr_str, "[");

	for (size_t i=0; i<json_array->current_index; i++) {
		if (!(tmp = json_obj_parser(json_array->json_obj_arr+i)))
			goto json_arr_exit;

		size += strlen(tmp);

		if (i != json_array->current_index-1)
			size+=2;

		if (size >= tmp_size) {
			tmp_size = STD_JSON_OBJ_SIZE+size;
			json_arr_str = realloc(json_arr_str, sizeof(char)*tmp_size);
		}

		strcat(json_arr_str, tmp);
		free(tmp);

		if (i != json_array->current_index-1)
			strcat(json_arr_str, ",");
	}

json_arr_exit:
	strcat(json_arr_str, "]\0");
	json_arr_str = realloc(json_arr_str, sizeof(char)*size);

	PRINT_ARR(size, json_arr_str)
	return json_arr_str;
}

void json_free(JSON *to_free) {
    PRINT_FREE_JSON(to_free)
    if (to_free) {
      free(to_free->json_name);
      to_free->json_name = NULL;

      if (to_free->kind == STRING)
        free(to_free->json_value);
      else if (to_free->kind == JSON_ARR)
        json_array_free(to_free->json_value);
      else if (to_free->kind == JSON_OBJ)
        json_object_free(to_free->json_value);

      if (to_free->kind != J_NULL && to_free->kind != BOOL && to_free->kind != NUMBER && to_free->kind != STRING)
        free(to_free->json_value);

      to_free->json_value = NULL;
    }
}

void json_object_free(JSONObject *to_free) {
	PRINT_FREE_OBJECT(to_free)
  if (!to_free)
		return;

 	for (size_t i=0; i<to_free->current_index; i++)
     json_free(to_free->json_arr+i);

 	free(to_free->json_arr);
 	to_free->json_arr = NULL;
}

void json_array_free(JSONArray *to_free) {
	PRINT_FREE_ARRAY(to_free)
	if (!to_free)
      return;

	for (size_t i=0; i<to_free->current_index; i++)
     json_object_free(to_free->json_obj_arr+i);

   free(to_free->json_obj_arr);
   to_free->json_obj_arr = NULL;
} 
