#include <stdlib.h>
#include <string.h>

#include "f_tree.h"

// Fonction que vérifie si l'arbre est correct (parenthèses bien fermées et
// expression ne commençant pas par '|', '*' ou ')' ).
// On utilise le boucle aussi pour compter le nombre de lettre que l'on aura 
// dans l'arbre (pour l'allocation de mémoire).
int verify_syntax_tree(char *expression) {
  int parenthesis = 0;         // Nombre de parenthèses ouvertes
  int letter = 0;             // Nombre de lettres dans l'arbre
  int operation = 0;         // Nombre d'opérations dans l'arbre

  // On coupe l'execution si l'expression est mal formée
  // Si non, on compte le nombre de lettre et parenthèses ouvertes
  switch (expression[0]) {
    case ')':
    case '|':
    case '*':
      fprintf(stderr, "Erreur : expression incorrecte.\n");
      abort();
    case '.':
      fprintf(stderr, "Erreur : caractère interdit.\n");
      abort();
    case '(':
      parenthesis++;
      operation++;
      break;
    default:
      letter++;
      break;
  }

  for (int i = 1; i < strlen(expression); i++) {
    char *curr_char = &expression[i];
    char *prev_char = &expression[i-1];

    switch (*curr_char) {
    case '(':
      parenthesis++;
      operation++;

      if (*prev_char != '|' && *prev_char != '(') {
        operation++;
      }
      break;
      
    case ')':
      parenthesis--;

      if (*prev_char == '|') {
        fprintf(stderr, "Erreur : expression incorrecte.\n");
        abort();
      }
      break;
    case '|':
      operation++;

      if (*prev_char == '|' || *prev_char == '(') {
        fprintf(stderr, "Erreur : expression incorrecte.\n");
        abort();
      }
      break;
    case '*':
      operation++;

      if (*prev_char == '|' || *prev_char == '(' || *prev_char == '*') {
        fprintf(stderr, "Erreur : expression incorrecte.\n");
        abort();
      }
      break;
    case '.':
      fprintf(stderr, "Erreur : caractère interdit (.).\n");
      abort();
    default:
      letter++;

      if (*prev_char != '|' && *prev_char != '(') {
        operation++;
      }
      break;
    }
  }

  if (parenthesis != 0) {
    fprintf(stderr, "Erreur : parenthèses non équilibrées.\n");
    abort();
  }

  return letter + operation;
}


// Fonction qui crée l'arbre syntaxique à partir de l'expression régulière.
// On utilise un tableau de noeuds pour stocker les feuilles et un tableau
// auxiliaire pour stocker les opérateurs avant les pousser dans le tableau 
// des noeuds.
SyntaxTree *create_syntax_tree(char *expression) {
  // On vérifie la syntaxe de l'expression et on compte le nombre de noeuds.
  int fail = 1;
  int nodes = verify_syntax_tree(expression);
  
  // On alloue la mémoire pour l'arbre syntaxique
  SyntaxTree *tree = malloc(sizeof(SyntaxTree));
  tree->root = NULL;
  tree->leaves = NULL;
  tree->num_leaves = nodes;

  // On alloue la mémoire pour les noeuds de l'arbre et un stack auxiliaire
  TreeNode **leaves = malloc(sizeof(TreeNode*) * nodes);
  TreeNode **aux_operator = malloc(sizeof(TreeNode*) * nodes);

  int leaf_index = -1;              // Index pour le tableau de noeuds
  int operator_index = -1;          // Index pour le tableau d'opérateurs

  if (expression[0] == '(') {
    operator_index++;
    aux_operator[operator_index] = malloc(sizeof(TreeNode));
    initialize_leaf(aux_operator[operator_index], ')', -1);
  } else {
    leaf_index++;
    leaves[leaf_index] = malloc(sizeof(TreeNode));
    initialize_leaf(leaves[leaf_index], expression[0], leaf_index);
  }

  for (int i = 1; i < strlen(expression); i++) {
    char *curr_char = &expression[i];       // Caractère courant
    char *prev_char = &expression[i-1];     // Caractère précédent

    switch (*curr_char) {
      case '(':
        if (*prev_char != '|' && *prev_char != '(') {
          operator_index++;
          aux_operator[operator_index] = malloc(sizeof(TreeNode));
          initialize_leaf(aux_operator[operator_index], '.', -1);
        } 
        operator_index++;
        aux_operator[operator_index] = malloc(sizeof(TreeNode));
        initialize_leaf(aux_operator[operator_index], ')', -1);
        break;
      case ')':
        while(aux_operator[operator_index] && aux_operator[operator_index]->value != ')') {
          if (aux_operator[operator_index] && aux_operator[operator_index]->value == '|') {
            leaf_index++;
            leaves[leaf_index] = malloc(sizeof(TreeNode));
            initialize_leaf(leaves[leaf_index], aux_operator[operator_index]->value, leaf_index);
            for (int j = leaf_index-1; j >= 0; j--) {
              if (leaves[j]->parent == NULL) {
                leaves[j]->parent = leaves[leaf_index];
                if (leaves[leaf_index]->right_child == NULL) {
                  leaves[leaf_index]->right_child = leaves[j];
                }
                else {
                  leaves[leaf_index]->left_child = leaves[j];
                  break;
                }
              }
            }

            free(aux_operator[operator_index]);
            aux_operator[operator_index] = NULL;
            operator_index--;
          }
        } 

        // On arrive à l'ouverture du parenthèse et on l'ajoute à l'arbre
        leaf_index++;
        leaves[leaf_index] = malloc(sizeof(TreeNode));
        initialize_leaf(leaves[leaf_index], aux_operator[operator_index]->value, leaf_index);
        leaves[leaf_index]->left_child = leaves[leaf_index-1];
        leaves[leaf_index-1]->parent = leaves[leaf_index];

        free(aux_operator[operator_index]);
        aux_operator[operator_index] = NULL;
        operator_index--;
        
        break;
      case '|':
        operator_index++;
        aux_operator[operator_index] = malloc(sizeof(TreeNode));
        initialize_leaf(aux_operator[operator_index], *curr_char, -1);
        break;
      case '*':
        leaf_index++;
        leaves[leaf_index] = malloc(sizeof(TreeNode));
        initialize_leaf(leaves[leaf_index], *curr_char, leaf_index);
        leaves[leaf_index]->left_child = leaves[leaf_index-1];
        leaves[leaf_index-1]->parent = leaves[leaf_index];

        if (*prev_char == ')') {
          if (aux_operator[operator_index] && aux_operator[operator_index]->value == '.') {
            leaf_index++;
            leaves[leaf_index] = malloc(sizeof(TreeNode));
            initialize_leaf(leaves[leaf_index], '.', leaf_index);

            for (int j = leaf_index-1; j >= 0; j--) {
              if (leaves[j]->parent == NULL) {
                leaves[j]->parent = leaves[leaf_index];
                if (leaves[leaf_index]->right_child == NULL) {
                  leaves[leaf_index]->right_child = leaves[j];
                }
                else {
                  leaves[leaf_index]->left_child = leaves[j];
                  break;
                }
              }
            }

            free(aux_operator[operator_index]);
            aux_operator[operator_index] = NULL;    
            operator_index--;
          }
        } 
        break;
      default:
        if (*prev_char == ')' && operator_index >= 0) {
          leaf_index++;
          leaves[leaf_index] = malloc(sizeof(TreeNode));
          initialize_leaf(leaves[leaf_index], aux_operator[operator_index]->value, leaf_index);

          if (aux_operator[operator_index] && aux_operator[operator_index]->value != ')') {
            for (int j = leaf_index-1; j >= 0; j--) {
              if (leaves[j]->parent == NULL) {
                leaves[j]->parent = leaves[leaf_index];
                if (leaves[leaf_index]->right_child == NULL) {
                  leaves[leaf_index]->right_child = leaves[j];
                }
                else {
                  leaves[leaf_index]->left_child = leaves[j];
                  break;
                }
              }
            }
          } 
          
          free(aux_operator[operator_index]);
          aux_operator[operator_index] = NULL;
          operator_index--;
        }

        leaf_index++;
        leaves[leaf_index] = malloc(sizeof(TreeNode));
        initialize_leaf(leaves[leaf_index], *curr_char, leaf_index);

        if (*prev_char == '|') {}
        else if (*prev_char != '(') {
          leaf_index++;
          leaves[leaf_index] = malloc(sizeof(TreeNode));
          initialize_leaf(leaves[leaf_index], '.', leaf_index);
          
          for (int j = leaf_index-1; j >= 0; j--) {
            if (leaves[j]->parent == NULL) {
              leaves[j]->parent = leaves[leaf_index];
              if (leaves[leaf_index]->right_child == NULL) {
                leaves[leaf_index]->right_child = leaves[j];
              }
              else {
                leaves[leaf_index]->left_child = leaves[j];
                break;
              }
            }
          }
        }
        break;
    }
    printf("iter %d, curr_char: %c\n", i, *curr_char);
    printf("\n\n");
  }

  while (operator_index >= 0) {
    leaf_index++;
    leaves[leaf_index] = malloc(sizeof(TreeNode));
    initialize_leaf(leaves[leaf_index], aux_operator[operator_index]->value, leaf_index);

    if (aux_operator[operator_index]) {
      for (int j = leaf_index-1; j >= 0; j--) {
        if (leaves[j]->parent == NULL) {
          leaves[j]->parent = leaves[leaf_index];
          if (leaves[leaf_index]->right_child == NULL) {
            leaves[leaf_index]->right_child = leaves[j];
          }
          else {
            leaves[leaf_index]->left_child = leaves[j];
            break;
          }
        }
      }
    }

    free(aux_operator[operator_index]);
    aux_operator[operator_index] = NULL;
    operator_index--;
  }
  
  tree->root = leaves[nodes-1];
  tree->leaves = leaves;

  display_tree(tree);
  free(aux_operator);

  return tree;
}


void initialize_leaf(TreeNode *leaf, char value, int index) {
  leaf->value = value;
  leaf->index = index;
  leaf->parent = NULL;
  leaf->left_child = NULL;
  leaf->right_child = NULL;
}


void display_tree(SyntaxTree *tree) {
  for (int i = 0; i < tree->num_leaves; i++) {
    if (tree->leaves[i]) {
      printf("Node %d:\n", i);
      printf("  index: %d\n", tree->leaves[i]->index);
      printf("  value: %c\n", tree->leaves[i]->value);
      printf("  parent: %c\n", (tree->leaves[i]->parent) ? tree->leaves[i]->parent->value : ' ');
      printf("  left_child: %c\n", (tree->leaves[i]->left_child) ? tree->leaves[i]->left_child->value : ' ');
      printf("  right_child: %c\n\n", (tree->leaves[i]->right_child) ? tree->leaves[i]->right_child->value : ' ');
    } else {
      printf("Node Unknown.\n\n");
    }
  }
}


void export_node(FILE *f, TreeNode *leaf) {
    if (!leaf) return;

    // Print current node
    fprintf(f, "  node%d [label=\"%d:  %c\"];\n", leaf->index, leaf->index+1, leaf->value);

    // Print left edge
    if (leaf->left_child) {
        fprintf(f, "  node%d -> node%d [label=\"\"];\n", leaf->index, leaf->left_child->index);
        export_node(f, leaf->left_child);
    }

    // Print right edge
    if (leaf->right_child) {
        fprintf(f, "  node%d -> node%d [label=\"\"];\n", leaf->index, leaf->right_child->index);
        export_node(f, leaf->right_child);
    }
}


void export_tree_to_graphviz(const char *filename, SyntaxTree *tree) {
    FILE* f = fopen(filename, "w");
    if (!f) {
        perror("Erreur d'ouverture de fichier.");
        return;
    }

    fprintf(f, "digraph SyntaxTree {\n");
    fprintf(f, "  node [shape=circle];\n");

    if (tree->root)
        export_node(f, tree->root);

    fprintf(f, "}\n");
    fclose(f);
}
