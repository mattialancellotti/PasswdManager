#ifndef PARSER_H
#define PARSER_H

#define boolean (0)
#define string  (1)
#define number  (2)

struct yaml_option {
  const char *yaml_key;
  const int   arg_type;
  const char  val;
};

#endif
