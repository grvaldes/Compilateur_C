#include <stdio.h>
#include <stdlib.h>

#include "f_aux_automata.h"

// Fonctions automates


void initialize_nfa(Automaton *nfa, SyntaxTree *tree) {
  nfa->num_states = 0;
  nfa->num_transitions = 0;
  nfa->num_unique_chars = tree->num_unique_chars;
  nfa->unique_chars = tree->unique_chars;

  for (int i = 0; i < tree->num_leaves; i++) {
    switch (tree->leaves[i]->value) {
      case '|':
        nfa->num_states = nfa->num_states + 2;
        nfa->num_transitions = nfa->num_transitions + 4;
        break;
      case '.':
        nfa->num_transitions = nfa->num_transitions + 1;
        break;
      case '*':
        nfa->num_states = nfa->num_states + 2;
        nfa->num_transitions = nfa->num_transitions + 4;
        break;
      case ')':
        break;
      default:
        nfa->num_states = nfa->num_states + 2;
        nfa->num_transitions = nfa->num_transitions + 1;
        break;
    }
  }

  nfa->states = malloc(sizeof(AState*) * nfa->num_states);
  nfa->transitions = malloc(sizeof(ATransition*) * nfa->num_transitions);
}

void initialize_state(AState* state, int index, int is_start, int is_final) {
  state->index = index;
  state->is_start = is_start;
  state->is_final = is_final;
  state->is_deleted = 0;
  state->num_transitions = 0;
  state->states_set = NULL;
  state->transitions = NULL;
}

void initialize_transition(ATransition *transition, AState *from, AState *to, char symbol) {
  transition->from = from;
  transition->to = to;
  transition->symbol = symbol;
}

void add_transition_to_state(ATransition *transition, AState *state) {
  state->num_transitions++;
  state->transitions = realloc(state->transitions, sizeof(ATransition*) * state->num_transitions);
  state->transitions[state->num_transitions - 1] = transition;
}

void add_transition_to_nfa(Automaton *automat, ATransition *transition, int counter) {
  automat->transitions[counter] = transition;
}


void add_state_to_nfa(Automaton *automat, AState *state, int counter, int position) {
  automat->states[counter] = state;
}


void merge_nodes_automaton(Automaton *automat) {
  for (int i=0; i < automat->num_transitions; i++) {
    if (automat->transitions[i]->symbol == '@') {
      AState* node_to_delete = automat->transitions[i]->from;
      AState* node_to_merge = automat->transitions[i]->to;

      for (int j=0; j < automat->num_transitions; j++) {
        if (automat->transitions[j]->to->index == node_to_delete->index) {
          automat->transitions[j]->to = node_to_merge;
        }
      }

      automat->transitions[i]->to = node_to_delete;
      node_to_delete->is_deleted = 1;
    }
  }
}


void export_automaton_to_graphviz(const char *filename, Automaton *automat) {
    FILE* f = fopen(filename, "w");
    if (!f) {
        perror("Erreur d'ouverture de fichier.");
        return;
    }

    fprintf(f, "digraph Automaton {\n");
    fprintf(f, "  rankdir=LR;\n");
    fprintf(f, "  node [shape=circle];\n");

    for (int i = 0; i < automat->num_states; ++i) {
        AState* state = automat->states[i];
        if (state->is_deleted) continue;
        if (state->is_final)
            fprintf(f, "  q%d [shape=doublecircle];\n", state->index);
        else
            fprintf(f, "  q%d;\n", state->index);
    }

    for (int i = 0; i < automat->num_states; ++i) {
        AState* state = automat->states[i];
        if (state->is_deleted) continue;
        if (state->is_start) {
            fprintf(f, "  start_q%d [shape=point];\n", state->index);
            fprintf(f, "  start_q%d -> q%d;\n", state->index, state->index);
        }
    }

    for (int i = 0; i < automat->num_transitions; ++i) {
        ATransition* t = automat->transitions[i];
        char label = t->symbol;
        if (label != '@') {
          fprintf(f, "  q%d -> q%d [label=\"%c\"];\n", t->from->index, t->to->index, label);
        }
    }

    fprintf(f, "}\n");
    fclose(f);
}


// DFA Functions
void follow_path_from_single_state(NodeSet *node_set, AState *initial_state, char value) {
  for (int i = 0; i < initial_state->num_transitions; i++) {
    if ((initial_state->transitions[i]->symbol == '#' && node_set->array_size > 0) || initial_state->transitions[i]->symbol == value) {
      if (!state_in_node_set(initial_state->transitions[i]->to->index, node_set)) {
        node_set->array_size++;
        node_set->node_array = realloc(node_set->node_array, sizeof(int) * (node_set->array_size));
        node_set->node_array[node_set->array_size-1] = initial_state->transitions[i]->to->index;
        follow_path_from_single_state(node_set, initial_state->transitions[i]->to, value);
      }
    }
  }
}


void follow_path_from_group_state(Automaton *dfa, Automaton *nfa, AState *initial_state, char value) {
  NodeSet *node_set = malloc(sizeof(NodeSet));
  node_set->array_size = 0;
  node_set->node_array = NULL;


  for (int i=0; i < initial_state->states_set->array_size; i++) {
    follow_path_from_single_state(node_set, nfa->states[initial_state->states_set->node_array[i]], value);
  }

  if (node_set->array_size == 0) return;

  for (int i=0; i < dfa->num_states; i++) {
    if(contained_node_set(dfa->states[i]->states_set, node_set)) {
      ATransition *transition = malloc(sizeof(ATransition));
      initialize_transition(transition, initial_state, dfa->states[i], value);
      add_transition_to_dfa(dfa, transition);
      return;
    } 
  }

  AState* new_state = malloc(sizeof(AState));
  ATransition *transition = malloc(sizeof(ATransition));
  initialize_transition(transition, initial_state, new_state, value);
  initialize_dfa_state(new_state, dfa->num_states, NOT_START, NOT_FINAL, node_set);
  add_state_to_dfa(dfa, new_state);
  add_transition_to_dfa(dfa, transition);
}


void initialize_dfa(Automaton *dfa, Automaton *nfa) {
  dfa->num_states = 0;
  dfa->num_transitions = 0;
  dfa->num_unique_chars = nfa->num_unique_chars;
  dfa->unique_chars = nfa->unique_chars;
  dfa->states = NULL;
  dfa->transitions = NULL;
}


void add_state_to_dfa(Automaton *automat, AState *state) {
  automat->num_states++;
  automat->states = realloc(automat->states, sizeof(AState*) * automat->num_states);
  automat->states[automat->num_states-1] = state;
}

void add_transition_to_dfa(Automaton *automat, ATransition *transition) {
  automat->num_transitions++;
  automat->transitions = realloc(automat->transitions, sizeof(ATransition*) * automat->num_transitions);
  automat->transitions[automat->num_transitions-1] = transition;
}


void initialize_dfa_state(AState *state, int index, int is_start, int is_final, NodeSet *node_set) {
  initialize_state(state, index, is_start, is_final);
  state->states_set = node_set;
}


int state_in_node_set(int index, NodeSet *node_set) {
  if(!node_set->node_array) return 0;
  for (int i=0; i < node_set->array_size; i++) {
    if (index == node_set->node_array[i]) return 1;
  }
  return 0;
}


int compare_node_sets(NodeSet *set1, NodeSet *set2) {
  if (set1->array_size != set2->array_size) return 0;
  int count = 0;
  for (int i=0; i < set1->array_size; i++) {
    for (int j=0; j < set2->array_size; j++) {
      if (set1->node_array[i] == set2->node_array[j]) {
        count++;
        break;
      }
    }
  }
  if (count == set1->array_size) return 1;
  else return 0;
}


int contained_node_set(NodeSet *set1, NodeSet *set2) {
  int count = 0;
  for (int i=0; i < set2->array_size; i++) {
    for (int j=0; j < set1->array_size; j++) {
      if (set2->node_array[i] == set1->node_array[j]) {
        count++;
        break;
      }
    }
  }
  if (count == set2->array_size) return 1;
  else return 0;
}


void check_final_states(Automaton *dfa, Automaton *nfa) {
  for (int i=0; i < dfa->num_states; i++) {
    for (int j=0; j < dfa->states[i]->states_set->array_size; j++) {
      if (nfa->states[dfa->states[i]->states_set->node_array[j]]->is_final) {
        dfa->states[i]->is_final = 1;
        break;
      }
    }
  }

  for (int i=0; i < dfa->num_transitions; i++) {
    add_transition_to_state(dfa->transitions[i], dfa->transitions[i]->from);
  }
}


// Fonctions DFA minimales
void remove_states_from_set(NodeSet *original_set, NodeSet *new_set) {
  for (int i=0; i < new_set->array_size; i++) {
    for (int j=0; j < original_set->array_size; j++) {
      if (new_set->node_array[i] == original_set->node_array[j]) {
        for (int k=j; k < original_set->array_size-1; k++) {
          original_set->node_array[k] = original_set->node_array[k+1];
        }
        original_set->array_size--;
        original_set->node_array = realloc(original_set->node_array, sizeof(NodeSet) * original_set->array_size);
      }
    }
  }
}