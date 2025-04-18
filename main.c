#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG
#define MAX_LINE_LENGTH 1024

#include "functions/f_tree.h"
#include "functions/f_nfa.h"

int main(int argc, char *argv[]) {
  // Vérification du nombre d'arguments
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return 1;
  }

  // Ouverture du fichier
  FILE *file = fopen(argv[1], "r");
  if (file == NULL) {
    // Vérification d'erreur d'ouverture du fichier
    fprintf(stderr, "Erreur d'ouverture du fichier.\n");
    return 1;
  }
  
  // Lecture de la première ligne du fichier
  // ATTENTION : Toutes les autres lignes sont ignorées donc il faut s'assurer 
  // que le fichier contient au une ligne delimité par un retour à la fin.
  char line[MAX_LINE_LENGTH];
  fgets(line, MAX_LINE_LENGTH, file);

  // Suppression des caractères inutiles après le retour.
  char* expression = strtok(line, "\n");
  fprintf(stdout, "L'expression régulière à analyser est : %s\n", expression);
  
  // Vérification de l'arbre syntaxique
  int nodes = verify_syntax_tree(expression);
  fprintf(stdout, "L'arbre a %d noeuds.\n", nodes);

  // Création de l'arbre syntaxique
  SyntaxTree *tree = create_syntax_tree(expression);

  // Création de l'automate non déterministe
  NFA *nfa = create_nfa_from_syntax_tree(tree);

  // Fermeture du fichier et libération de la mémoire
  free(tree->leaves);
  free(tree);
  fclose(file);

  return 0;
}