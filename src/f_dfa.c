#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "f_dfa.h"
#include "f_aux_automata.h"

Automaton *create_dfa_from_nfa(Automaton *nfa) {
  // Variables auxiliaires
  // On alloue la mémoire pour les objets
  int state_index = 0;
  Automaton *dfa = malloc(sizeof(Automaton));  
  AState *initial_state = malloc(sizeof(AState));
  NodeSet *initial_set = malloc(sizeof(NodeSet));

  initialize_dfa(dfa, nfa);
  
  // On cherche l'état de début
  for (int i=0; i < nfa->num_states; i++) {
    if (nfa->states[i]->is_start) {
      *initial_state = *nfa->states[i];
      break;
    }
  }

  // On cherche le premier état en trouvant la epsilon-cloture
  initial_set->node_array = malloc(sizeof(int));
  initial_set->array_size = 1;
  initial_set->node_array[0] = initial_state->index;
  
  follow_path_from_single_state(initial_set, initial_state, '#');
  initialize_dfa_state(initial_state, state_index, IS_START, NOT_FINAL, initial_set);
  add_state_to_dfa(dfa, initial_state);

  // À partir de le premier état, on cherche le parcours de chaque groupe de états pour chaque lettre
  do {
    initial_state = dfa->states[state_index++];
    for (int k=0; k < strlen(nfa->unique_chars); k++)
      follow_path_from_group_state(dfa, nfa, initial_state, nfa->unique_chars[k]);
  } while (dfa->num_states != state_index);

  // On cherche les états que contient des état finaux de le NFA
  check_final_states(dfa, nfa);

  return dfa;
}