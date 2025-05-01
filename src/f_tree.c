#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "f_tree.h"
#include "f_aux_tree.h"

#define ASCII_SIZE 128


// Fonction qui crée l'arbre syntaxique à partir de l'expression régulière.
// On utilise un tableau de noeuds pour stocker les feuilles et un tableau
// auxiliaire pour stocker les opérateurs avant les pousser dans le tableau 
// des noeuds.
SyntaxTree *create_syntax_tree(char *expression) {
  // On vérifie la syntaxe de l'expression et on compte le nombre de noeuds.
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

  // On analyse le premier caractère de l'expression
  if (expression[0] == '(') {
    aux_operator[++operator_index] = malloc(sizeof(TreeNode));
    initialize_leaf(aux_operator[operator_index], ')', -1);
  } else {
    leaves[++leaf_index] = malloc(sizeof(TreeNode));
    initialize_leaf(leaves[leaf_index], expression[0], leaf_index);
  }

  // On analyse chaque caractère de l'expression
  for (int i = 1; i < strlen(expression); i++) {
    char *curr_char = &expression[i];       // Caractère courant
    char *prev_char = &expression[i-1];     // Caractère précédent

    // Pour le caractère actuel
    switch (*curr_char) {
      case '(':
        // Si l'on ouvre un parenthèse, on l'ajoute à la queue des opérateurs
        // Si le parenthèse vient après un étoile ou une lettre, on fait un concatenation implicite avant
        if (*prev_char != '|' && *prev_char != '(') {
          aux_operator[++operator_index] = malloc(sizeof(TreeNode));
          initialize_leaf(aux_operator[operator_index], '.', -1);
        } 
        aux_operator[++operator_index] = malloc(sizeof(TreeNode));
        initialize_leaf(aux_operator[operator_index], ')', -1);
        break;
      case ')':
        // Si l'on ferme le parenthèse, on procese tous les opérateurs dans la queue jusqu'à on arrive au parenthèse ouvert
        while(operator_index >= 0 && aux_operator[operator_index] && aux_operator[operator_index]->value != ')') {
          // Si l'operateur est union, on cherche dans la queue des lettres les deux dernières noeuds sans parent
          if (operator_index >= 0 && aux_operator[operator_index] && 
              (aux_operator[operator_index]->value == '|' || aux_operator[operator_index]->value == '.')) {
            leaves[++leaf_index] = malloc(sizeof(TreeNode));
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
            aux_operator[operator_index--] = NULL;
          }
        } 

        // On arrive à l'ouverture du parenthèse et on l'ajoute à l'arbre
        leaves[++leaf_index] = malloc(sizeof(TreeNode));
        initialize_leaf(leaves[leaf_index], aux_operator[operator_index]->value, leaf_index);
        leaves[leaf_index]->left_child = leaves[leaf_index-1];
        leaves[leaf_index-1]->parent = leaves[leaf_index];

        free(aux_operator[operator_index]);
        aux_operator[operator_index--] = NULL;    
        break;
      case '|':
        // Si le caractère est union, on l'ajout a la queue des opérateurs
        aux_operator[++operator_index] = malloc(sizeof(TreeNode));
        initialize_leaf(aux_operator[operator_index], *curr_char, -1);
        break;
      case '*':
        // Si le caractère est l'étoile de Kleene on l'ajoute directement à l'arbre
        leaves[++leaf_index] = malloc(sizeof(TreeNode));
        initialize_leaf(leaves[leaf_index], *curr_char, leaf_index);
        leaves[leaf_index]->left_child = leaves[leaf_index-1];
        leaves[leaf_index-1]->parent = leaves[leaf_index];

        // Si le caractère precedent est parenthèse et on a une concatenation dans la queue, on ajoute
        if (*prev_char == ')') {
          if (operator_index >= 0 && aux_operator[operator_index] && aux_operator[operator_index]->value == '.') {
            leaves[++leaf_index] = malloc(sizeof(TreeNode));
            initialize_leaf(leaves[leaf_index], '.', leaf_index);

            // On trouve les deux dernières noeuds sans parent
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
            aux_operator[operator_index--] = NULL;
          }
        } 
        break;
      default:
        // Si le caractère est une lettre, on analyse avant le caractère precedent
        // Si caractère est parenthèse ferme et on a des opérateurs dans la queue, on ajoute le lettre et l’opérateur a l'arbre
        if (*prev_char == ')' && operator_index >= 0) {
          leaves[++leaf_index] = malloc(sizeof(TreeNode));
          initialize_leaf(leaves[leaf_index], aux_operator[operator_index]->value, leaf_index);

          // On cherche les dernières noeuds sans parent
          if (operator_index >= 0 && aux_operator[operator_index] && aux_operator[operator_index]->value != ')') {
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
          aux_operator[operator_index--] = NULL;
        }

        // On ajoute le lettre
        leaves[++leaf_index] = malloc(sizeof(TreeNode));
        initialize_leaf(leaves[leaf_index], *curr_char, leaf_index);

        // Si le caractère precedent est étoile ou lettre, on ajoute une concatenation
        if (*prev_char == '|') {}
        else if (*prev_char != '(') {
          leaves[++leaf_index] = malloc(sizeof(TreeNode));
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
  }

  // S'il reste des opérateurs après finir avec les lettres, on les ajoute.
  while (operator_index >= 0) {
    leaves[++leaf_index] = malloc(sizeof(TreeNode));
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
    aux_operator[operator_index--] = NULL;
  }

  // On cherche les caractères uniques dans l'expression
  char *unique_chars = malloc(sizeof(char) * ASCII_SIZE);
  int *out_count = malloc(sizeof(int));
  find_unique_alphanumerics(expression, unique_chars, out_count);
  
  // On ajoute tout a l'objet Arbre
  tree->root = leaves[nodes-1];
  tree->leaves = leaves;
  tree->num_unique_chars = *out_count;
  tree->unique_chars = unique_chars;

  free(aux_operator);

  return tree;
}