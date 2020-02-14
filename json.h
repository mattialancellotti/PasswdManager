#ifndef JSON_H
#define JSON_H

#define JSON_SIZE_TIME 5
#define STD_JSON_OBJ_SIZE 50
#define ALLOCATED_SPACE 100
#define DEV_MODE

#ifdef DEV_MODE
	#define PRINT_INIT(x) printf("[%d] %s(void): Size: %ld\n", __LINE__, __func__, x);
	#define PRINT_JSON(x, y) printf("[%d] %s: Creating json key: %ld - %s\n", __LINE__, __func__, x, y);
	#define PRINT_OBJ(x, y)  printf("[%d] %s: Creating json object: %ld - %s\n", __LINE__,__func__, x, y);
	#define PRINT_ARR(x, y)  printf("[%d] %s: Creating json array: %ld - %s\n", __LINE__,__func__, size, json_arr_str);
	#define PRINT_ADDRS_ALLOC(x, y, z, j) printf("[%d] %s(size_t, enum MEM_KIND): Pos: %ld - Size: %ld - Kind: %d - Addrs: %p\n", __LINE__, __func__,  x, y, z, j);
	#define PRINT_ADDRS_REALLOC(x, y) printf("[%d] %s(void *, const size_t, size_t): Pos: %d - Kind: %p\n", __LINE__, __func__, x, y);
	#define PRINT_FREE_JSON(x) printf("[%d] %s(JSONObject *): %p\n", __LINE__, __func__, (void *)x);
	#define PRINT_FREE_OBJECT(x) printf("[%d] %s(JSONObject *): %p\n", __LINE__, __func__, (void *)x);
	#define PRINT_FREE_ARRAY(x) printf("[%d] %s(JSONArray*): %p\n", __LINE__, __func__, (void *)x);
#else
	#define PRINT_JSON(x, y) 
	#define PRINT_OBJ(x, y) 
	#define PRINT_ARR(x, y)
	#define PRINT_ADDRS_ALLOC(x, y, z, j) 
	#define PRINT_ADDRS_REALLOC(x, y)
	#define PRINT_FREE_JSON(x)
	#define PRINT_FREE_OBJECT(x)
	#define PRINT_FREE_ARRAY(x)
	#define PRINT_INIT(x)
#endif

enum KIND     { STRING, NUMBER,   BOOL,        JSON_OBJ,   JSON_ARR, J_NULL };
enum MEM_KIND { STR,    JSON_KEY, JSON_OBJECT, JSON_ARRAY, O_NULL           };

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

void *addrs_alloc(size_t, enum MEM_KIND);
void *addrs_realloc(void *, const size_t);
int addrs_search(const void *);
void addrs_arr_alloc(void);

JSON *json_put(char *, void *, enum KIND);
JSONObject *json_object_add(JSON *, JSONObject *);
JSONArray *json_array_add(JSONObject *, JSONArray *);

char *json_parser(JSON *);
char *json_obj_parser(JSONObject *);
char *json_arr_parser(JSONArray *);

void json_free(JSON *);
void json_object_free(JSONObject *);
void json_array_free(JSONArray *);

char *json_indent(char *);

#endif
