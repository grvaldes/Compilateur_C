#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "f_dfa.h"

#define IS_FINAL 1
#define IS_START 1
#define NOT_FINAL 0
#define NOT_START 0
#define AT_START 0
#define AT_END 1

Automaton *create_dfa_from_nfa(Automaton *nfa) {
  int state_index = 0;
  Automaton *dfa = malloc(sizeof(Automaton));
  AState *initial_state = malloc(sizeof(AState));
  NodeSet *initial_set = malloc(sizeof(NodeSet));
  
  // Start from first state.
  for (int i=0; i < nfa->num_states; i++) {
    if (nfa->states[i]->is_start) {
      initial_state = nfa->states[i];
      break;
    }
  }

  // Find route of e-transitions. State 0
  initial_set->node_array = malloc(sizeof(int));
  initial_set->array_size = 1;
  initial_set->node_array[0] = initial_state->index;
  follow_automaton_path(initial_set, initial_state, '#');

  for (int i=0; i < initial_set->array_size; i++) {
    printf("%d(%d)   ", i, initial_set->node_array[i]);
  }

  initialize_dfa_state(initial_state, state_index++, IS_START, NOT_FINAL, initial_set);
  // dfa_states[0] = malloc(sizeof(AState));

  // (for) From state 0, find every char-transition
    // (If) transition new, create new state, point transition
    // (Else) repeated, point transition to existing
  // (endFor) When no more new states
  // Add states and transitions to dfa
  dfa = NULL;
  return dfa;
}

void *follow_automaton_path(NodeSet *node_set, AState *state, char value) {
  printf("node_set[%d]: %d\n", node_set->array_size-1, node_set->node_array[node_set->array_size-1]);

  for (int i = 0; i < state->num_transitions; i++) {
    printf("transition: %c\n", state->transitions[i]->symbol);
    if (state->transitions[i]->symbol == value) {
      printf("receiving index: %d\n", state->transitions[i]->to->index);
      if (!state_in_node_set(state->transitions[i]->to->index, node_set->node_array)) {
        node_set->array_size++;
        node_set->node_array = realloc(node_set->node_array, sizeof(int) * (node_set->array_size));
        node_set->node_array[node_set->array_size-1] = state->transitions[i]->to->index;
        printf("node_set[%d]: %d\n", node_set->array_size-1, node_set->node_array[node_set->array_size-1]);
        follow_automaton_path(node_set, state->transitions[i]->to, value);
      }
    }
  }
  return node_set;
}


void initialize_dfa_state(AState *state, int index, int is_start, int is_final, NodeSet *node_set) {
  initialize_state(state, index, is_start, is_final);
  state->states_set = node_set;
}


int state_in_node_set(int index, int *node_set) {
  for (int i=0; i < sizeof(*node_set)/sizeof(int); i++) {
    if (index == node_set[i]) return 1;
  }
  return 0;
}