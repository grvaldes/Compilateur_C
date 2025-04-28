#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "f_aux_tree.h"

// Fonction que vérifie si l'arbre est correct (parenthèses bien fermées et
// expression ne commençant pas par '|', '*' ou ')' ).
// On utilise le boucle aussi pour compter le nombre de lettre que l'on aura 
// dans l'arbre (pour l'allocation de mémoire).
int verify_syntax_tree(char *expression) {
  int parenthesis = 0;         // Nombre de parenthèses ouvertes
  int letter = 0;              // Nombre de lettres dans l'arbre
  int operation = 0;           // Nombre d'opérations dans l'arbre

  
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


// Fonction que initialise l'objet arbre.
void initialize_leaf(TreeNode *leaf, char value, int index) {
  leaf->value = value;
  leaf->index = index;
  leaf->parent = NULL;
  leaf->left_child = NULL;
  leaf->right_child = NULL;
}


// Fonction que cherche les caractères uniques dans l'expression
void find_unique_alphanumerics(const char* input, char* output, int* out_count) {
    int seen[128] = {0};
    int count = 0;

    for (int i = 0; input[i] != '\0'; i++) {
        char c = input[i];
        if (isalnum(c) && seen[c] == 0) {
            seen[c] = 1;
            output[count++] = c;
        }
    }
    output[count] = '\0';
    if (out_count) *out_count = count;

    input = realloc(output, sizeof(char) * *out_count);
}


// Fonction que écrit le node sur un fichier
void export_node(FILE *f, TreeNode *leaf) {
    if (!leaf) return;

    // Écrit le node actuel
    fprintf(f, "  node%d [label=\"%d:  %c\"];\n", leaf->index, leaf->index+1, leaf->value);

    // Écrit le feuille gauche
    if (leaf->left_child) {
        fprintf(f, "  node%d -> node%d [label=\"\"];\n", leaf->index, leaf->left_child->index);
        export_node(f, leaf->left_child);
    }

    // Écrit le feuille droite
    if (leaf->right_child) {
        fprintf(f, "  node%d -> node%d [label=\"\"];\n", leaf->index, leaf->right_child->index);
        export_node(f, leaf->right_child);
      }
}


// Fonction qui écrit le fichier .dot pour la representation graphique de l'arbre
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


