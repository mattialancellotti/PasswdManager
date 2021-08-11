#define _XOPEN_SOURCE 500
#if defined(__debug__)
#  include <assert.h>
#endif

#include <string.h>
#include <yaml.h>

#include <pass/yaml_parser.h>
#include <pass/tree.h>

/*
 * This function parses any kind of yaml file and saves the content in a N-ary
 * tree (that is a tree with n nodes per each (sub)-root).
 *
 * In this case the function uses the implementation from glib to parse the
 * yaml file and then returns the root of the tree back to the caller function.
 */
static TreeNode *process_block(TreeNode* /*parent*/, yaml_parser_t* /*parser*/,
                                 yaml_event_type_t /*prev_event*/);

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

   /* Creatin the tree */
   TreeNode *tree = process_block(NULL, &parser, YAML_NO_EVENT);

   call(t_print_structure(tree));

   report("Tree has been created.");
   yaml_parser_delete(&parser);

   t_tree_destroy(tree);
   report("Tree has been deleted.");
   return NULL;
}

static TreeNode *process_block(TreeNode *parent, yaml_parser_t *parser, 
                                 yaml_event_type_t prev_event)
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
         if (prev_event == YAML_SCALAR_EVENT)
            /* Adding a new child to the current parent */
            parent = t_insert_node(parent, 
               strdup((const char *)event.data.scalar.value));
         printf("> %s\n", (char *)event.data.scalar.value);
      } else if (event.type == YAML_MAPPING_START_EVENT) {
         parent = t_insert_node(parent, NULL);
         TreeNode *tmp_node = parent->child;

         /* Checks if the current context is a list of yaml `objects` */
         if (prev_event == YAML_SEQUENCE_START_EVENT) {
            parent->child = process_block(tmp_node, parser, event.type);
         } else {
            t_cycle_through_all(tmp_node);
            tmp_node = process_block(tmp_node, parser, event.type);
         }

      } else if (event.type == YAML_SEQUENCE_START_EVENT) {
         parent = process_block(parent->child, parser, event.type);
      }

      if (event.type == YAML_NO_EVENT)
         break;
      printf("%d\n", event.type);
      /* Freeing memory */
      if (event.type != YAML_STREAM_END_EVENT  &&
          event.type != YAML_SEQUENCE_END_EVENT)
         yaml_event_delete(&event);

   } while (event.type != YAML_STREAM_END_EVENT  && 
            event.type != YAML_SEQUENCE_END_EVENT);
   yaml_event_delete(&event);

   /* Returning the (sub)-tree */
   return parent;
}
