#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "f_dfa.h"
#include "f_aux_automata.h"

Automaton *create_dfa_from_nfa(Automaton *nfa) {
  int state_index = 0;
  Automaton *dfa = malloc(sizeof(Automaton));  
  AState *initial_state = malloc(sizeof(AState));
  NodeSet *initial_set = malloc(sizeof(NodeSet));

  initialize_dfa(dfa, nfa);
  
  // Start from first state.
  for (int i=0; i < nfa->num_states; i++) {
    if (nfa->states[i]->is_start) {
      *initial_state = *nfa->states[i];
      break;
    }
  }

  // Find route of e-transitions. State 0
  initial_set->node_array = malloc(sizeof(int));
  initial_set->array_size = 1;
  initial_set->node_array[0] = initial_state->index;
  
  follow_path_from_single_state(initial_set, initial_state, '#');
  initialize_dfa_state(initial_state, state_index, IS_START, NOT_FINAL, initial_set);
  add_state_to_dfa(dfa, initial_state);

  printf("\n\n\n\n\n\n\n");

  // for (int i=0; i < initial_set->array_size; i++) {
  //   printf("%d(%d)   ", i, initial_set->node_array[i]);
  // }

  // (for) From state 0, find every char-transition
  do {
    initial_state = dfa->states[state_index];
    printf("iter %d after initial state\n", state_index);

    
    for (int k=0; k < strlen(nfa->unique_chars); k++) {
      printf("%c\n",nfa->unique_chars[k]);
      follow_path_from_group_state(dfa, nfa, initial_state, nfa->unique_chars[k]);

    }
    printf("iter %d after for loop\n", state_index);

    state_index++;
  } while (dfa->num_states != state_index);

  // Check for final states
  check_final_states(dfa, nfa);

  return dfa;
}