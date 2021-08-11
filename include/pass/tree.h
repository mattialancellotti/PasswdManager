#ifndef TREE_H
#define TREE_H

typedef void* tpointer;

typedef struct Node {
  tpointer data;

  struct Node *next;
  struct Node *prev;
  struct Node *parent;
  struct Node *child;
} TreeNode;

/* TODO doc */
TreeNode *t_init();

/* TODO:
 *    - Rename this to t_insert
 *    - Insert a new node at the last pos
 */
TreeNode *t_insert_node(TreeNode* /*parent*/, tpointer /*data*/);

/* TODO:
 *    - Create t_insert_at(parent, position, data)
 */

/*  TODO:
 *    - Only a define for t_insert_at
 */
TreeNode *t_insert_brother(TreeNode* /*brother*/, TreeNode* /*new_one*/);
void t_tree_destroy(TreeNode* /*root*/);

/*
 * TODO:
 *    - Rename to t_count_children
 */
int t_n_children(TreeNode* /*parent*/);

/* TODO update print macros to accept types */
#define t_print_structure(x); t_print_all(x, 0, 0, -1);
#define t_print_leaves(x); t_print_all(x, 0, t_struct_analyzer(x, 2)-1, -1);
#define t_print(x); t_print_all(x, 0, 0, 0);
#define t_cycle_through_all(x); while (x && x->next) x = x->next;
#define t_get_brother_data(x);
#define t_jump_next_keyword(x); \
  for (int i=0; i<2 && x && x->next; i++, x = x->next) ;

int t_struct_analyzer(TreeNode* /*root*/, unsigned /*flag*/);
void t_print_all(TreeNode* /*root*/, int /*curr_layer*/, 
                    int /*first_layer*/, int /*depth*/);
void t_print_root(TreeNode* /*root*/);

#endif
