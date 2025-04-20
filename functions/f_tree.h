#ifndef F_TREE_H
#define F_TREE_H

#include <stdio.h>

// Noeud de l'arbre syntaxique
typedef struct TreeNode {
  char value;                     // Caractère de l'arbre
  int index;                      // Index du noeud dans l'arbre
  struct TreeNode *parent;        // Pointeur vers le parent.
  struct TreeNode *left_child;    // Pointeur vers le fils gauche ou unique.
  struct TreeNode *right_child;   // Pointeur vers fils droite ou NULL
} TreeNode;

// Arbre syntaxique
typedef struct SyntaxTree {
  int num_leaves;                 // Nombre de feuilles dans l'arbre
  TreeNode *root;                 // Pointeur vers la racine de l'arbre
  TreeNode **leaves;              // Tableau de pointeurs vers les feuilles (ordonnées).
} SyntaxTree;

// Fonctions
int verify_syntax_tree(char *expression);
SyntaxTree *create_syntax_tree(char *expression);

void initialize_leaf(TreeNode *leaf, char value, int index);
void display_tree(SyntaxTree *tree);

void export_node(FILE *f, TreeNode *node);
void export_tree_to_graphviz(const char *filename, SyntaxTree *tree);

#endif