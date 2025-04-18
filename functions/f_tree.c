#include <stdio.h>
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
    char *prev_char = &expression[i - 1];

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
  int nodes = verify_syntax_tree(expression);
  
  // On alloue la mémoire pour l'arbre syntaxique
  SyntaxTree *tree = malloc(sizeof(SyntaxTree));
  tree->root = NULL;
  tree->leaves = NULL;
  tree->num_leaves = nodes;

  // On alloue la mémoire pour les noeuds de l'arbre et un stack auxiliaire
  TreeNode *leaves = malloc(sizeof(TreeNode) * nodes);
  TreeNode *aux_operator = malloc(sizeof(TreeNode) * nodes);

  int leaf_index = -1;              // Index pour le tableau de noeuds
  int operator_index = -1;          // Index pour le tableau d'opérateurs

  // On initialise le tableau de noeuds. Si l'expression commence par une parenthèse,
  // on l'ajoute au tableau d'opérateurs. Sinon, on l'ajoute au tableau de noeuds.
  // L'indice est -1 quand on ne sait pas où le placer dans l'arbre.
  // Les parents et enfants sont encore pas connus.
  switch(expression[0]) {
    case '(':
      operator_index++;
      aux_operator[operator_index].value = ')';
      aux_operator[operator_index].index = -1;
      aux_operator[operator_index].parent = NULL;
      aux_operator[operator_index].left_child = NULL;
      aux_operator[operator_index].right_child = NULL;
      break;
    default:
      leaf_index++;
      leaves[leaf_index].value = expression[0];
      leaves[leaf_index].index = leaf_index;
      leaves[leaf_index].parent = NULL;
      leaves[leaf_index].left_child = NULL;
      leaves[leaf_index].right_child = NULL;
      break;
  }
  
  // On parcourt l'expression régulière caractère par caractère
  // et on crée les noeuds de l'arbre syntaxique.
  for (int i = 1; i < strlen(expression); i++) {
    char *curr_char = &expression[i];       // Caractère courant
    char *prev_char = &expression[i-1];     // Caractère précédent

    switch (*curr_char) {
      // Si le caractère courant est une parenthèse ouvrante, on l'ajoute au tableau
      // d'opérateurs. Si le caractère précédent n'est pas un opérateur, on ajoute
      // un opérateur de concaténation '.' au tableau de noeuds.
      case '(':
        if (*prev_char != '|' && *prev_char != '(') {
          operator_index++;
          aux_operator[operator_index].value = '.';
          aux_operator[operator_index].index = -1;
          aux_operator[operator_index].parent = NULL;
          aux_operator[operator_index].left_child = NULL;
          aux_operator[operator_index].right_child = NULL;
        }
        
        operator_index++;
        aux_operator[operator_index].value = ')';
        aux_operator[operator_index].index = -1;
        aux_operator[operator_index].parent = NULL;
        aux_operator[operator_index].left_child = NULL;
        aux_operator[operator_index].right_child = NULL;
        break;
      // Si le caractère courant est une parenthèse fermante, on dépile les opérateurs
      // jusqu'à ce qu'on trouve une parenthèse ouvrante.
      case ')':
        while(aux_operator[operator_index].value != ')') {
          leaf_index++;
          if (aux_operator[operator_index].value == '|') {
            leaves[leaf_index] = aux_operator[operator_index];
            leaves[leaf_index].index = leaf_index;
            leaves[leaf_index].left_child = &leaves[leaf_index-2]; 
            leaves[leaf_index].right_child = &leaves[leaf_index-1];
            leaves[leaf_index-2].parent = &leaves[leaf_index];
            leaves[leaf_index-1].parent = &leaves[leaf_index];
          }   
          operator_index--;
        } 
        leaf_index++;
        leaves[leaf_index] = aux_operator[operator_index];
        leaves[leaf_index].index = leaf_index;
        leaves[leaf_index].left_child = &leaves[leaf_index-1];
        leaves[leaf_index-1].parent = &leaves[leaf_index];
        operator_index--;
        break;
      // Si le caractère courant est un conjonction, on l'ajoute au tableau d'opérateurs.
      case '|':
        operator_index++;
        aux_operator[operator_index].value = *curr_char;
        aux_operator[operator_index].index = -1;
        aux_operator[operator_index].parent = NULL;
        aux_operator[operator_index].left_child = NULL;
        aux_operator[operator_index].right_child = NULL;
        break;
      // Si le caractère courant est une étoile, on l'ajoute au tableau de noeuds.
      // Si le caractère précédent est une parenthèse fermante, on ajoute un opérateur
      // de concaténation '.' au tableau de noeuds.
      case '*':
        leaf_index++;
        leaves[leaf_index].value = *curr_char;
        leaves[leaf_index].index = leaf_index;
        leaves[leaf_index].parent = NULL;
        leaves[leaf_index].left_child = &leaves[leaf_index-1];
        leaves[leaf_index].right_child = NULL;
        leaves[leaf_index-1].parent = &leaves[leaf_index];

        if (*prev_char == ')') {
          if (aux_operator[operator_index].value == '.') {
            leaf_index++;
            leaves[leaf_index].value = '.';
            leaves[leaf_index].index = leaf_index;
            leaves[leaf_index].parent = NULL;
            leaves[leaf_index].left_child = NULL;
            leaves[leaf_index].right_child = NULL;

            for (int j = leaf_index-1; j >= 0; j--) {
              if (leaves[j].parent == NULL) {
                leaves[j].parent = &leaves[leaf_index];
                if (leaves[leaf_index].right_child == NULL) {
                  leaves[leaf_index].right_child = &leaves[j];
                }
                else {
                  leaves[leaf_index].left_child = &leaves[j];
                  break;
                }
              }
            }
            operator_index--;
          }
        } 
        break;
      // Si le caractère courant est une lettre, on l'ajoute au tableau de noeuds.
      // Si le caractère précédent n'est pas un opérateur ou est une parenthèse fermante,
      // on ajoute un opérateur de concaténation '.' au tableau de noeuds.
      default:
        if (*prev_char == ')' && operator_index >= 0) {
          leaf_index++;
          leaves[leaf_index].value = aux_operator[operator_index].value;
          leaves[leaf_index].index = leaf_index;
          leaves[leaf_index].parent = NULL;
          leaves[leaf_index].left_child = NULL;
          leaves[leaf_index].right_child = NULL;

          if (aux_operator[operator_index].value == '.') {
            for (int j = leaf_index-1; j >= 0; j--) {
              if (leaves[j].parent == NULL) {
                leaves[j].parent = &leaves[leaf_index];
                if (leaves[leaf_index].right_child == NULL) {
                  leaves[leaf_index].right_child = &leaves[j];
                }
                else {
                  leaves[leaf_index].left_child = &leaves[j];
                  break;
                }
              }
            }
          } else if (aux_operator[operator_index].value == '|') {
            leaves[leaf_index].left_child = &leaves[leaf_index-2];
            leaves[leaf_index].right_child = &leaves[leaf_index-1];
            leaves[leaf_index-2].parent = &leaves[leaf_index];
            leaves[leaf_index-1].parent = &leaves[leaf_index];
          }
          operator_index--;
        }

        leaf_index++;
        leaves[leaf_index].value = *curr_char;
        leaves[leaf_index].index = leaf_index;
        leaves[leaf_index].parent = NULL;
        leaves[leaf_index].left_child = NULL;
        leaves[leaf_index].right_child = NULL;

        if (*prev_char == '|') {}
        else if (*prev_char != '(') {
          leaf_index++;
          leaves[leaf_index].value = '.';
          leaves[leaf_index].index = leaf_index;
          leaves[leaf_index].parent = NULL;
          leaves[leaf_index].left_child = NULL;
          leaves[leaf_index].right_child = NULL;

          for (int j = leaf_index-1; j >= 0; j--) {
            if (leaves[j].parent == NULL) {
              leaves[j].parent = &leaves[leaf_index];
              if (leaves[leaf_index].right_child == NULL) {
                leaves[leaf_index].right_child = &leaves[j];
              }
              else {
                leaves[leaf_index].left_child = &leaves[j];
                break;
              }
            }
          }
        }
        break;
    }
    // printf("iter %d, curr_char: %c\n", i, *curr_char);
    // for (int j = 0; j < nodes; j++) {
    //   printf("%c", leaves[j].value);
    // }
    // printf("\n");
    // for (int j = 0; j < nodes; j++) {
    //   printf("%c", aux_operator[j].value);
    // }
    // printf("\n\n");
  }

  // On pointe la racine de l'arbre vers le dernier noeud du tableau de noeuds
  // et on pointe le tableau de noeuds. On libère la mémoire du tableau d'opérateurs.
  tree->root = &leaves[nodes-1];
  tree->leaves = leaves;
  // for (int i = 0; i < tree->num_leaves; i++) {
  //   printf("Node %d: %c\t%d\t%d\t%d\n", 
  //     tree->leaves[i].index+1, 
  //     tree->leaves[i].value,
  //     tree->leaves[i].parent == NULL ? -1 : tree->leaves[i].parent->index+1,
  //     tree->leaves[i].left_child == NULL ? -1 : tree->leaves[i].left_child->index+1,
  //     tree->leaves[i].right_child == NULL ? -1 : tree->leaves[i].right_child->index+1);
  // }
  // for (int i = 0; i < operator_index+1; i++) {
  //   printf("Operator %d: %c\t%d\t%d\t%d\n", 
  //     aux_operator[i].index+1, 
  //     aux_operator[i].value,
  //     aux_operator[i].parent == NULL ? -1 : aux_operator[i].parent->index+1,
  //     aux_operator[i].left_child == NULL ? -1 : aux_operator[i].left_child->index+1,
  //     aux_operator[i].right_child == NULL ? -1 : aux_operator[i].right_child->index+1);
  // }
  free(aux_operator);
  

  // On retourne l'arbre syntaxique
  return tree;
}