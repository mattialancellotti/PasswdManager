#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
/* TODO Adjust those macro thesting things and DEBUG */
#include <assert.h>
#include <string.h>

#include <pass/tree.h>

#define t_init_node(data, var); \
   var = t_new_node(data, NULL, NULL);
#define t_brand_new_brother(data, parent, prev, var_name); \
   TreeNode *var_name = t_new_node(data, parent, NULL); \
   t_create_siblings(prev, var_name);
#define t_init_new_child(data, parent); \
   parent->child = t_new_node(data, parent, NULL);
#define t_init_anonymous_child(parent, child); \
   parent->child = t_new_node(NULL, parent, child);

static void t_create_siblings(TreeNode* /*first*/, TreeNode* /*second*/);
static TreeNode *t_new_node(tpointer /*data*/, TreeNode* /*parent*/,
                              TreeNode* /*child*/);
static TreeNode *t_last_sibling(TreeNode* /*child*/);
static void t_num_layer(TreeNode* /*tree*/, int /*curr_layer*/,
                                 int* /*max_layer*/);
static void t_heighest_leaves(TreeNode* /*tree*/, int /*curr_layer*/,
                                 int* /*min_layer*/);

TreeNode *t_insert_node(TreeNode *parent, tpointer data)
{
   if (parent == NULL) {
      t_init_node(data, parent);
   } else if (parent->child) {
      TreeNode *curr_child = parent->child;
      t_cycle_through_all(curr_child);
      t_brand_new_brother(data, curr_child->parent, curr_child, tmp_node);
   } else {
      t_init_new_child(data, parent);
   }

   assert(parent != NULL);
   return parent;
}

static TreeNode *t_new_node(tpointer data, TreeNode *parent, TreeNode *child)
{
   /* Creating and initializing the new node */
   TreeNode *new_node = malloc(sizeof(TreeNode));

   if (new_node) {
      new_node->data = data;
      new_node->parent = parent;
      new_node->child = child;
      new_node->next = new_node->prev = NULL;
   }

  /* Checking and returning it */
  assert(new_node != NULL);
  return new_node;
}

void t_tree_destroy(TreeNode *root)
{
   if (root == NULL)
      return;

   if (root->child)
      t_tree_destroy(root->child);
      
   if (root->next)
      t_tree_destroy(root->next);
      
   free(root->data);
   free(root);
   root = NULL;
}

int t_n_children(TreeNode *parent)
{
   if (parent == NULL || parent->child == NULL)
      return 0;

   TreeNode *child = parent->child;
   int count = 1;
   while (child->next && count++)
      ;

   return count;
}

static void t_create_siblings(TreeNode *first, TreeNode *second)
{
   /* Updating sibling connection */
   first->next = second;
   second->prev = first;
}

TreeNode *t_insert_brother(TreeNode *brother, TreeNode *new_one)
{
   if (brother == NULL || new_one == NULL)
      return NULL;

   if (brother->parent == NULL) {
      brother->parent = (TreeNode *)malloc(sizeof(TreeNode));

      brother->parent->child = brother;
      brother->parent->data = NULL;

      if (brother->next == NULL)
         t_create_siblings(brother, new_one);
      else {
         new_one->next = brother->next;
         brother->next = new_one;
      }

      new_one->parent = brother->parent;
      return brother->parent;
   }

   if (brother->next == NULL)
      t_create_siblings(brother, new_one);
   else {
      new_one->next = brother->next;
      brother->next = new_one;
   }

   return brother;
}

void t_print_all(TreeNode *root, int curr_layer, int first_layer, int depth)
{
   if (root == NULL)
      return;

   /* Reaching the right layer */
   if (curr_layer < first_layer) {
      t_print_all(root->child, curr_layer+1, first_layer, depth);
      t_print_all(root->next, curr_layer, first_layer, depth);
   } else if (depth == -1 || curr_layer <= first_layer+depth) {
      printf("Layer [%d]", curr_layer);
      if (root->parent && root->parent->data)
         printf("(^ %10s): ", (char *)root->parent->data);
      else
         printf("(^ %10s): ", "nil");
      /* Trying to do some pretty print with long string */
      if (root->data) {
         if (strlen(root->data) >= 20) {
            /* Initializing the new string */
            char shorter_str[20];
            strncpy(shorter_str, root->data, 6);
            strcat(shorter_str, "..");

            printf("%s\n", shorter_str);
         } else
            printf("%s\n", (char *)root->data);
     } else
        printf("nil\n");

     /* Loops only if required */
     if (depth == -1 || curr_layer < first_layer+depth) {
       t_print_all(root->child, curr_layer+1, first_layer, depth);
       t_print_all(root->next, curr_layer, first_layer, depth);
     }
   }
}

void t_print_root(TreeNode *root)
{
   if (root == NULL)
      return;

   while (root->parent)
      root = root->parent;

   if (root->data != NULL)
      printf("Root: %s\n", ((char *)root->data));
   else
      printf("Root: nil\n");
}

static TreeNode *t_last_sibling(TreeNode *child)
{
   if (child == NULL)
      return NULL;

   while (child->next)
      child = child->next;

   return child;
}

int t_struct_analyzer(TreeNode *root, unsigned flag)
{
   if (root == NULL)
      return -1;

   int layer = 0;

   if (flag & 1)
      t_num_layer(root, 0, &layer);
   else if (flag & 2)
      t_heighest_leaves(root, 0, &layer);

   return layer;
}

static void t_num_layer(TreeNode *tree, int curr_layer, int *max_layer)
{
   /* Updating the max layer */
   if (curr_layer > *max_layer)
      *max_layer = curr_layer;

   /* Checking for children to loop through */
   if (tree->child) {
      *max_layer += 1;
      t_num_layer(tree->child, curr_layer+1, max_layer);
   }

   /* Checking for brothers */
   if (tree->next)
      t_num_layer(tree->next, curr_layer, max_layer);
}

static void t_heighest_leaves(TreeNode *tree, int curr_layer, int *min_layer)
{

   if (tree->child) {
      *min_layer += 1;
      t_heighest_leaves(tree->child, curr_layer+1, min_layer);
   } else {
      /* Updating the min layer */
      if (curr_layer < *min_layer)
         *min_layer = curr_layer;
   }

   if (tree->next)
      t_heighest_leaves(tree->next, curr_layer, min_layer);
}
