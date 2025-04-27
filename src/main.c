#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

#include "f_aux_tree.h"
#include "f_aux_automata.h"
#include "f_tree.h"
#include "f_nfa.h"
#include "f_dfa.h"
#include "f_dfa_min.h"

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
  export_tree_to_graphviz("files/tree.dot", tree);
  system("dot -Tpng files/tree.dot -o files/tree.png");

  // Création de l'automate non déterministe
  Automaton *nfa = create_nfa_from_syntax_tree(tree);
  export_automaton_to_graphviz("files/nfa.dot", nfa);
  system("dot -Tpng files/nfa.dot -o files/nfa.png");
  fprintf(stdout, "L'automate non déterministe a %d états et %d transitions.\n", nfa->num_states, nfa->num_transitions);

  // Création de l'automate déterministe
  Automaton *dfa = create_dfa_from_nfa(nfa);
  export_automaton_to_graphviz("files/dfa.dot", dfa);
  system("dot -Tpng files/dfa.dot -o files/dfa.png");
  fprintf(stdout, "L'automate déterministe a %d états et %d transitions.\n", dfa->num_states, dfa->num_transitions);

  // Création de l'automate déterministe minimal
  Automaton *dfa_m = create_minimal_dfa(dfa);
  export_automaton_to_graphviz("files/dfa_min.dot", dfa_m);
  system("dot -Tpng files/dfa_min.dot -o files/dfa_min.png");
  fprintf(stdout, "L'automate non déterministe minimale a %d états et %d transitions.\n", dfa_m->num_states, dfa_m->num_transitions);

  // Création du code C pour reconnaître le langage
  // export_automate_code(dfa_m)
  
  fclose(file);

  return 0;
}