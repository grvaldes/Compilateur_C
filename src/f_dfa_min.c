#include <stdlib.h>
#include <string.h>

#include "f_aux_automata.h"
#include "f_dfa_min.h"

Automaton *create_minimal_dfa(Automaton *dfa) {
  NodeSet **all_sets = NULL;
  Automaton *dfa_min = malloc(sizeof(Automaton));
  NodeSet *accept_set = malloc(sizeof(NodeSet));
  NodeSet *initial_set = malloc(sizeof(NodeSet));
  
  initialize_dfa(dfa_min, dfa);
  initial_set->node_array = NULL;
  initial_set->array_size = 0;  
  accept_set->node_array = NULL;
  accept_set->array_size = 0;
  
  // 1. Find all acceptant nodes
  for (int i=0; i < dfa->num_states; i++) {
    if (dfa->states[i]->is_final) {
      accept_set->array_size++;
      accept_set->node_array = realloc(accept_set->node_array, sizeof(int) * accept_set->array_size);
      accept_set->node_array[accept_set->array_size-1] = dfa->states[i]->index;
    } else {
      initial_set->array_size++;
      initial_set->node_array = realloc(initial_set->node_array, sizeof(int) * initial_set->array_size);
      initial_set->node_array[initial_set->array_size-1] = dfa->states[i]->index;
    }
  }
  
  int num_sets = 2;
  int num_initial_groups;
  all_sets = realloc(all_sets, sizeof(NodeSet*) * num_sets);
  all_sets[0] = accept_set;
  all_sets[1] = initial_set;
  
  // While we keep adding groups
  do {
    num_initial_groups = num_sets;
    // For all sets
    for (int i=0; i < num_initial_groups; i++) {
      NodeSet *curr_set = all_sets[i];
      NodeSet *new_set = malloc(sizeof(NodeSet));
      new_set->array_size = 0;
      new_set->node_array = NULL;

      if (curr_set->array_size == 1) continue;

      // For all states in set
      for (int j=0; j < curr_set->array_size; j++) {
        AState* curr_state = dfa->states[curr_set->node_array[j]];

        int count = 0;
        // For each letter
        for (int k=0; k < dfa->num_unique_chars; k++) {
          for (int t=0; t < curr_state->num_transitions; t++) {
            if (curr_state->transitions[t]->symbol == dfa->unique_chars[k]) {
              if (state_in_node_set(curr_state->transitions[t]->to->index, curr_set)) {
                count++;
              }
            }
          }
        }

        if (count == dfa->num_unique_chars) {
          continue;
        } else{
          new_set->array_size++;
          new_set->node_array = realloc(new_set->node_array, sizeof(NodeSet) * new_set->array_size);
          new_set->node_array[new_set->array_size-1] = curr_state->index;
        }
      }

      if (new_set->array_size > 0 && !compare_node_sets(curr_set, new_set)) {
        num_sets++;
        all_sets = realloc(all_sets, sizeof(NodeSet*) * num_sets);
        all_sets[num_sets-1] = new_set;

        remove_states_from_set(curr_set, new_set);
      }
    }
  } while (num_sets != num_initial_groups);

  // Create states from sets
  for (int i=0; i < num_sets; i++) {
    AState *curr_state = malloc(sizeof(AState));
    int is_start = 0;
    int is_final = 0;

    for (int j=0; j < all_sets[i]->array_size; j++) {
      if (dfa->states[all_sets[i]->node_array[j]]->is_start) {
        is_start = 1;
      }
      if (dfa->states[all_sets[i]->node_array[j]]->is_final) {
        is_final = 1;
      }
    }

    initialize_dfa_state(curr_state, i, is_start, is_final, all_sets[i]);
    add_state_to_dfa(dfa_min, curr_state);
  }

  // Consolidate transitions
  int check[dfa_min->num_states][dfa_min->num_unique_chars];
  for (int i=0; i < dfa_min->num_states; i++)
    for (int j=0; j < dfa_min->num_unique_chars; j++)
      check[i][j] = 0;


  for (int i=0; i < dfa->num_transitions; i++) {
    AState* from = dfa->transitions[i]->from;
    AState* to = dfa->transitions[i]->to;
    char symbol = dfa->transitions[i]->symbol;
    char* loc = strchr(dfa_min->unique_chars, symbol);
    int letter_index = (int)(loc - dfa_min->unique_chars);
    int found_from = 0;
    int found_to = 0;

    ATransition *new_trans = malloc(sizeof(ATransition));
    new_trans->symbol = symbol;
    
    for (int j=0; j < dfa_min->num_states; j++) {
      if (found_from && found_to) break;
      if (!found_from) {
        for (int k=0; k < dfa_min->states[j]->states_set->array_size; k++) {
          if (from->index == dfa_min->states[j]->states_set->node_array[k]) {
            new_trans->from = dfa_min->states[j];
            found_from = 1;
            break;
          }
        }
      }
      if (!found_to) {
        for (int k=0; k < dfa_min->states[j]->states_set->array_size; k++) {
          if (to->index == dfa_min->states[j]->states_set->node_array[k]) {
            new_trans->to = dfa_min->states[j];
            found_to = 1;
            break;
          }
        }
      }
    }

    if (check[new_trans->from->index][letter_index] == 0 && (new_trans->from->num_transitions <= dfa_min->num_unique_chars)) {
      check[new_trans->from->index][letter_index] = 1;
      add_transition_to_state(new_trans, new_trans->from);
      add_transition_to_dfa(dfa_min, new_trans);
    }
  }

  return dfa_min;
}