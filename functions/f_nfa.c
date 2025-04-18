#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "f_nfa.h"

#define IS_FINAL 1
#define IS_START 1
#define NOT_FINAL 0
#define NOT_START 0
#define AT_START 0
#define AT_END 1

NFA *create_nfa_from_syntax_tree(SyntaxTree *tree) {
  // Compteurs
  int index = 0;
  int index_initial[2];
  int index_final[2];

  index_initial[0] = 0;
  index_initial[1] = 0;
  index_final[0] = 0;
  index_final[1] = 0;

  int transition_counter = 0;
  int state_counter = 0;

  // On alloue la mémoire pour les automates. On considère que chaque
  // feuille de l'arbre syntaxique correspond à un automate.
  NFA *return_nfa = malloc(sizeof(NFA));
  initialize_nfa(return_nfa, tree);

  // On modifie l'automate a avec chaque feuille de l'arbre syntaxique.
  for (int i = 0; i < tree->num_leaves; i++) {
    printf("Leaf %d: %c\n", i, tree->leaves[i].value);
    switch (tree->leaves[i].value) {
      case '.':
        // Créer un NFA pour l'opération de concaténation
        return_nfa->states[index_initial[1]].index = return_nfa->states[index_final[0]].index;
        return_nfa->states[index_final[0]].is_final = 0;
        return_nfa->states[index_initial[1]].is_start = 0;

        // for (int i = 0; return_nfa->num_states; i++) {
        //   if (return_nfa->states[i].index > return_nfa->states[index_initial[1]].index) {
        //     return_nfa->states[i].index--;
        //   }
        // }

        index_initial[1] = index_initial[0];
        index_final[0] = index_final[1];

        break;
      case '|':
        // Créer un NFA pour l'opération de choix
        // On initialise le premier et le dernier état de l'automate es ses 4 transitions.
        // Ces états sont les états non hérités des automates nfa1 et nfa2.
        {
          NFAState *state0 = malloc(sizeof(NFAState));
          NFAState *state1 = malloc(sizeof(NFAState));
          NFATransition *trans0 = malloc(sizeof(NFATransition));
          NFATransition *trans1 = malloc(sizeof(NFATransition));
          NFATransition *trans2 = malloc(sizeof(NFATransition));
          NFATransition *trans3 = malloc(sizeof(NFATransition));

          initialize_state(state0, index++, 2, IS_START, NOT_FINAL);
          initialize_state(state1, index++, 0, NOT_START, IS_FINAL);
          initialize_transition(trans0, state0, &return_nfa->states[index_initial[0]], ' ');
          initialize_transition(trans1, state0, &return_nfa->states[index_initial[1]], ' ');
          initialize_transition(trans2, &return_nfa->states[index_final[0]], state1, ' ');
          initialize_transition(trans3, &return_nfa->states[index_final[1]], state1, ' ');

          add_transition_to_state(trans0, state0);
          add_transition_to_state(trans1, state0);
          add_transition_to_state(trans2, &return_nfa->states[index_final[0]]);
          add_transition_to_state(trans3, &return_nfa->states[index_final[1]]);

          add_transition_to_nfa(return_nfa, trans0, transition_counter++);
          add_transition_to_nfa(return_nfa, trans1, transition_counter++);
          add_transition_to_nfa(return_nfa, trans2, transition_counter++);
          add_transition_to_nfa(return_nfa, trans3, transition_counter++);

          add_state_to_nfa(return_nfa, state0, AT_START, state_counter++);
          add_state_to_nfa(return_nfa, state1, AT_END, state_counter++);

          return_nfa->states[index_initial[0]].is_start = 0;
          return_nfa->states[index_initial[1]].is_start = 0;
          return_nfa->states[index_final[0]].is_final = 0;
          return_nfa->states[index_final[1]].is_final = 0;

          index_initial[0] = index_initial[1];
          index_final[0] = index_final[1];
          index_initial[1] = state0->index;
          index_final[1] = state1->index;
        }
        break;
      case '*':
        // Créer un NFA pour l'opération de fermeture de Kleene
        // On initialise le premier et le dernier état de l'automate es ses 4 transitions.
        // Ces états sont les états non hérités des automates nfa1 et nfa2.
        {         
          NFAState *state0 = malloc(sizeof(NFAState));
          NFAState *state1 = malloc(sizeof(NFAState));
          NFATransition *trans0 = malloc(sizeof(NFATransition));
          NFATransition *trans1 = malloc(sizeof(NFATransition));
          NFATransition *trans2 = malloc(sizeof(NFATransition));
          NFATransition *trans3 = malloc(sizeof(NFATransition));

          initialize_state(state0, index++, 2, IS_START, NOT_FINAL);
          initialize_state(state1, index++, 0, NOT_START, IS_FINAL);
          initialize_transition(trans0, state0, &return_nfa->states[index_initial[1]], ' ');
          initialize_transition(trans1, state0, state1, ' ');
          initialize_transition(trans2, &return_nfa->states[index_final[1]], &return_nfa->states[index_initial[1]], ' ');
          initialize_transition(trans3, &return_nfa->states[index_final[1]], state1, ' ');

          add_transition_to_state(trans0, state0);
          add_transition_to_state(trans1, state0);
          add_transition_to_state(trans2, &return_nfa->states[index_final[1]]);
          add_transition_to_state(trans3, &return_nfa->states[index_final[1]]);

          add_transition_to_nfa(return_nfa, trans0, transition_counter++);
          add_transition_to_nfa(return_nfa, trans1, transition_counter++);
          add_transition_to_nfa(return_nfa, trans2, transition_counter++);
          add_transition_to_nfa(return_nfa, trans3, transition_counter++);

          add_state_to_nfa(return_nfa, state0, AT_START, state_counter++);
          add_state_to_nfa(return_nfa, state1, AT_END, state_counter++);

          return_nfa->states[index_initial[1]].is_start = 0;
          return_nfa->states[index_final[1]].is_final = 0;

          index_initial[0] = index_initial[1];
          index_final[0] = index_final[1];
          index_initial[1] = state0->index;
          index_final[1] = state1->index;
        }
        break;
      case ')':
        // Créer un NFA pour l'opération de parenthèse.
        // On fait rien.
        break;
      default:
        // Créer un NFA pour une lettre. NFA a deux états et une transition.
        // On alloue la mémoire pour les attributs de l'automate.
        {
          NFAState *state0 = malloc(sizeof(NFAState));
          NFAState *state1 = malloc(sizeof(NFAState));
          NFATransition *transition0 = malloc(sizeof(NFATransition));

          // On initialise les états et la transition.
          initialize_state(state0, index++, 1, IS_START, NOT_FINAL);
          initialize_state(state1, index++, 0, NOT_START, IS_FINAL);
          initialize_transition(transition0, state0, state1, tree->leaves[i].value);

          // On pointe les transitions dans les états.
          add_transition_to_state(transition0, state0);

          // Ajouter les états et transitions à l'automate.
          add_transition_to_nfa(return_nfa, transition0, transition_counter++);
          add_state_to_nfa(return_nfa, state0, AT_END, state_counter++);
          add_state_to_nfa(return_nfa, state1, AT_END, state_counter++);

          index_initial[0] = index_initial[1];
          index_final[0] = index_final[1];
          index_initial[1] = state0->index;
          index_final[1] = state1->index;
        }
        break;
    }
  // display_nfa(nfas[i]);
  }
  display_nfa(return_nfa);

  return return_nfa;
}


void display_nfa(NFA *nfa) {
  for (int i = 0; i < nfa->num_states; i++) {
    printf("State %d:%c%c\n  out:\n    ", nfa->states[i].index, nfa->states[i].is_final ? 'F' : ' ',nfa->states[i].is_start ? 'I' : ' ');
    for (int j = 0; j < nfa->states[i].num_transitions; j++) {
      printf(" %d(%c)", nfa->states[i].transitions[j].to->index,nfa->states[i].transitions[j].symbol);
    }
    printf("\n\n");
  }

  for (int i = 0; i < nfa->num_states; i++) {
    printf("Transition %d(%c):\n %d    %d\n", i, nfa->transitions[i].symbol, nfa->transitions[i].from->index, nfa->transitions[i].to->index);
    printf("\n\n");
  }
}


void initialize_nfa(NFA *return_nfa, SyntaxTree *tree) {
  return_nfa->num_states = 0;
  return_nfa->num_transitions = 0;

  for (int i = 0; i < tree->num_leaves; i++) {
    switch (tree->leaves[i].value) {
      case '|':
        return_nfa->num_states = return_nfa->num_states + 2;
        return_nfa->num_transitions = return_nfa->num_transitions + 4;
        break;
      case '.':
        return_nfa->num_states = return_nfa->num_states - 1;
        break;
      case '*':
        return_nfa->num_states = return_nfa->num_states + 2;
        return_nfa->num_transitions = return_nfa->num_transitions + 4;
        break;
      case ')':
        break;
      default:
        return_nfa->num_states = return_nfa->num_states + 2;
        return_nfa->num_transitions = return_nfa->num_transitions + 1;
        break;
    }
  }

  return_nfa->states = malloc(sizeof(NFAState) * return_nfa->num_states);
  return_nfa->transitions = malloc(sizeof(NFATransition) * return_nfa->num_transitions);
}

void initialize_state(NFAState* state, int index, int num_transitions, int is_start, int is_final) {
  state->index = index;
  state->is_start = is_start;
  state->is_final = is_final;
  state->num_transitions = 0;
  state->transitions = NULL;
  }
}

void initialize_transition(NFATransition* transition, NFAState* from, NFAState* to, char symbol) {
  transition->from = from;
  transition->to = to;
  transition->symbol = symbol;
}

void add_transition_to_state(NFATransition* transition, NFAState* state) {
  state->num_transitions++;
  state->transitions = realloc(state->transitions, sizeof(NFATransition) * state->num_transitions);
  state->transitions[state->num_transitions - 1] = *transition;
}

void add_transition_to_nfa(NFA* nfa, NFATransition* transition, int counter) {
  nfa->transitions[counter] = *transition;
}


void add_state_to_nfa(NFA* nfa, NFAState* state, int position, int counter) {
  if (!position) {
    for (int i = 0; i < nfa->num_states; i++) {
      nfa->states[i].index++;
    }
    state->index = 0;
  }

  nfa->states[counter] = *state;
}