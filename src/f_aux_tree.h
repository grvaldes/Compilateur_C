#ifndef F_AUX_TREE_H
#define F_AUX_TREE_H

#include <stdio.h>
#include "aux_structs.h"

int verify_syntax_tree(char *expression);

void initialize_leaf(TreeNode *leaf, char value, int index);

void export_node(FILE *f, TreeNode *node);
void export_tree_to_graphviz(const char *filename, SyntaxTree *tree);

void find_unique_alphanumerics(const char* input, char* output, int* out_count);

#endif