#ifndef F_TREE_H
#define F_TREE_H

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
  TreeNode *root;                 // Pointeur vers la racine de l'arbre
  TreeNode *leaves;               // Tableau de pointeurs vers les feuilles (ordonnées).
  int num_leaves;                 // Nombre de feuilles dans l'arbre
} SyntaxTree;

// Fonctions
int verify_syntax_tree(char *expression);
SyntaxTree *create_syntax_tree(char *expression);
void export_tree_graph(SyntaxTree *tree, char *filename);

#endif