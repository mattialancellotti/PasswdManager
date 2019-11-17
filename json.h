#ifndef JSON_H
#define JSON_H

#define JSON_SIZE_TIME 5
#define STD_JSON_OBJ_SIZE 100
#define DEV_MODE 

enum KIND { STRING, NUMBER, BOOL, JSON_OBJ, JSON_ARR, J_NULL };

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
	enum KIND kind;
};

struct JSONArray {
	JSONObject *json_obj_arr;
	size_t size, current_index;
};

JSON *json_put(char *, void *, enum KIND);
JSONObject *json_object_add(JSON *, JSONObject *);
JSONArray *json_array_add(JSONObject *, JSONArray *);
char *json_parser(JSON *);
char *json_obj_parser(JSONObject *);
char *json_arr_parser(JSONArray *);

#endif
