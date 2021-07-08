#ifndef YAML_PARSER_H
#define YAML_PARSER_H

#include <main.h>

#if defined(__debug__)
# define report(msg); \
    printf("%s [%d]: In func. %s, \"%s\"\n", __FILE__, __LINE__, __func__, msg);
#else
# define report(msg);
#endif

const config_t *parse_settings(const char* /*file*/);

#endif
