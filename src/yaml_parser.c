#define _XOPEN_SOURCE 500
#if defined(__debug__)
#  include <assert.h>
#endif

#include <string.h>
#include <yaml.h>

#include <pass/yaml_parser.h>
#include <pass/tree.h>
#include <yaml/parser.h>

/*
 * This function parses any kind of yaml file and saves the content in a N-ary
 * tree (that is a tree with n nodes per each (sub)-root).
 *
 * In this case the function uses the implementation from glib to parse the
 * yaml file and then returns the root of the tree back to the caller function.
 */
static TreeNode *process_block(TreeNode* /*parent*/, yaml_parser_t* /*parser*/);

const config_t *parse_settings(const char *file)
{
   /* Initializing the yaml parser */
   yaml_parser_t parser;

   if (!yaml_parser_initialize(&parser))
      perror("Initialization failed\n");

   yaml_parser_set_input_string(&parser,
         (const unsigned char *)file, strlen(file));

   report("Initialization successful.");
   report("Attempt at creating the tree.");

   const struct yaml_option (*yaml_settings_options)[] = &(struct yaml_option[]){
      {"profiles_enabled",   boolean, 'e'},
      {"accept_same_passwd", boolean, 'a'},
      {"passwd_min_len", number, 'i'},
      {"passwd_max_len", number, 'x'},
      {"passwds_file", string, 'p'},
      {0,              0,       0 }
   };

   printf("%s\n", (*yaml_settings_options)[1].yaml_key);

   /* Creatin the tree */
   TreeNode *tree = process_block(NULL, &parser);

#if defined(__debug__)
   t_print_structure(tree);
#endif
   report("Tree has been created.");
   yaml_parser_delete(&parser);

   /* Parsing infos */
  // config_t *settings = malloc(sizeof(config_t));

   t_tree_destroy(tree);
   report("Tree has been deleted.");
   return NULL;
}

static TreeNode *process_block(TreeNode *parent, yaml_parser_t *parser)
{
   /* Defining an event */
   yaml_event_t event;

   /* 
    * This block of code cycles through every event found by the parser to
    * create a tree. Every time a `scalar` is found a new child is added to the
    * tree. On the other hand when a `mapping` or `sequence` event is found a
    * new function instance is created.
    */
   do {
      /* Parsing the file */
      yaml_parser_parse(parser, &event);

      /* Creating the tree by checking which event occurred */
      if (event.type == YAML_SCALAR_EVENT) {
         /* Adding a new child to the current parent */
         report("YAML_SCALAR_EVENT");
         parent = t_insert_node(parent, 
               strdup((const char *)event.data.scalar.value));
      } else if (parent &&
            (event.type == YAML_MAPPING_START_EVENT ||
             event.type == YAML_SEQUENCE_START_EVENT)) {
        /* 
         * Checks for children. If anyone occurs then the new brother is
         * added at the end of the `line`.
         */
         if (parent->child == NULL) {
            parent = process_block(parent, parser);
         } else if (parent->child) {
            TreeNode *tmp_child = parent->child;
            t_cycle_through_all(tmp_child);

            tmp_child = process_block(tmp_child, parser);
         }
         
         break;
      }

      if (event.type != YAML_STREAM_END_EVENT)
         yaml_event_delete(&event);

   } while (event.type != YAML_STREAM_END_EVENT);

   /* Returning the (sub)-tree */
   return parent;
}
