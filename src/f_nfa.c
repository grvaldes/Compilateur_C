#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aux_structs.h"
#include "f_aux_automata.h"

Automaton *create_nfa_from_syntax_tree(SyntaxTree *tree) {
  // Compteurs
  int index = 0;
  AState** initial_node = malloc(sizeof(AState*) * tree->num_leaves);
  AState** final_node = malloc(sizeof(AState*) * tree->num_leaves);

  int transition_counter = 0;
  int state_counter = 0;

  // On alloue la mémoire pour les automates. On considère que chaque
  // feuille de l'arbre syntaxique correspond à un automate.
  Automaton *nfa = malloc(sizeof(Automaton));
  initialize_nfa(nfa, tree);

  // On modifie l'automate a avec chaque feuille de l'arbre syntaxique.
  for (int i = 0; i < tree->num_leaves; i++) {
    switch (tree->leaves[i]->value) {
      case '.':
        {
          // Trouver les enfants
          int child_index_left = tree->leaves[i]->left_child->index;
          int child_index_right = tree->leaves[i]->right_child->index;
          
          // Créer un Automaton pour l'opération de concaténation
          ATransition* trans0 = malloc(sizeof(ATransition));

          // On ajoute les transitions et les états au NFA
          initialize_transition(trans0, final_node[child_index_left], initial_node[child_index_right], '@');
          add_transition_to_state(trans0, final_node[child_index_left], initial_node[child_index_right]);
          add_transition_to_nfa(nfa, trans0, transition_counter++);

          final_node[child_index_left]->is_final = 0;
          initial_node[child_index_right]->is_start = 0;

          // On marque les états initiaux et finaux du sous-nfa.
          initial_node[i] = initial_node[child_index_left];
          final_node[i] = final_node[child_index_right];
        }
        break;
      case '|':
        // Créer un Automaton pour l'opération de choix
        // On initialise le premier et le dernier état de l'automate es ses 4 transitions.
        // Ces états sont les états non hérités des automates nfa1 et nfa2.
        {
          int child_index_left = tree->leaves[i]->left_child->index;
          int child_index_right = tree->leaves[i]->right_child->index;
          
          AState *state0 = malloc(sizeof(AState));
          AState *state1 = malloc(sizeof(AState));
          ATransition *trans0 = malloc(sizeof(ATransition));
          ATransition *trans1 = malloc(sizeof(ATransition));
          ATransition *trans2 = malloc(sizeof(ATransition));
          ATransition *trans3 = malloc(sizeof(ATransition));

          // On ajoute les transitions et les états au NFA
          initialize_state(state0, index++, IS_START, NOT_FINAL);
          initialize_state(state1, index++, NOT_START, IS_FINAL);
          initialize_transition(trans0, state0, initial_node[child_index_left], '#');
          initialize_transition(trans1, state0, initial_node[child_index_right], '#');
          initialize_transition(trans2, final_node[child_index_left], state1, '#');
          initialize_transition(trans3, final_node[child_index_right], state1, '#');

          add_transition_to_state(trans0, state0, initial_node[child_index_left]);
          add_transition_to_state(trans1, state0, initial_node[child_index_right]);
          add_transition_to_state(trans2, final_node[child_index_left], state1);
          add_transition_to_state(trans3, final_node[child_index_right], state1);

          add_transition_to_nfa(nfa, trans0, transition_counter++);
          add_transition_to_nfa(nfa, trans1, transition_counter++);
          add_transition_to_nfa(nfa, trans2, transition_counter++);
          add_transition_to_nfa(nfa, trans3, transition_counter++);

          add_state_to_nfa(nfa, state0, state_counter++);
          add_state_to_nfa(nfa, state1, state_counter++);

          initial_node[child_index_left]->is_start = 0;
          initial_node[child_index_right]->is_start = 0;
          final_node[child_index_left]->is_final = 0;
          final_node[child_index_right]->is_final = 0;

          // On marque les états initiaux et finaux du sous-nfa.
          initial_node[i] = state0;
          final_node[i] = state1;
        }
        break;
      case '*':
        // Créer un Automaton pour l'opération de fermeture de Kleene
        // On initialise le premier et le dernier état de l'automate es ses 4 transitions.
        // Ces états sont les états non hérités des automates nfa1 et nfa2.
        {    
          int child_index = tree->leaves[i]->left_child->index;
          
          AState *state0 = malloc(sizeof(AState));
          AState *state1 = malloc(sizeof(AState));
          ATransition *trans0 = malloc(sizeof(ATransition));
          ATransition *trans1 = malloc(sizeof(ATransition));
          ATransition *trans2 = malloc(sizeof(ATransition));
          ATransition *trans3 = malloc(sizeof(ATransition));

          // On ajoute les transitions et les états au NFA
          initialize_state(state0, index++, IS_START, NOT_FINAL);
          initialize_state(state1, index++, NOT_START, IS_FINAL);
          initialize_transition(trans0, state0, initial_node[child_index], '#');
          initialize_transition(trans1, state0, state1, '#');
          initialize_transition(trans2, final_node[child_index], initial_node[child_index], '#');
          initialize_transition(trans3, final_node[child_index], state1, '#');

          add_transition_to_state(trans0, state0, initial_node[child_index]);
          add_transition_to_state(trans1, state0, state1);
          add_transition_to_state(trans2, final_node[child_index], initial_node[child_index]);
          add_transition_to_state(trans3, final_node[child_index], state1);

          add_state_to_nfa(nfa, state0, state_counter++);
          add_state_to_nfa(nfa, state1, state_counter++);
          
          add_transition_to_nfa(nfa, trans0, transition_counter++);
          add_transition_to_nfa(nfa, trans1, transition_counter++);
          add_transition_to_nfa(nfa, trans2, transition_counter++);
          add_transition_to_nfa(nfa, trans3, transition_counter++);

          initial_node[child_index]->is_start = 0;
          final_node[child_index]->is_final = 0;

          // On marque les états initiaux et finaux du sous-nfa.
          initial_node[i] = state0;
          final_node[i] = state1;
        }
        break;
      case ')':
        // Créer un Automaton pour l'opération de parenthèse.
        // On marque les états initiaux et finaux du sous-nfa.
        initial_node[i] = initial_node[i-1];
        final_node[i] = final_node[i-1];
        break;
      default:
        // Créer un Automaton pour une lettre. Automaton a deux états et une transition.
        // On alloue la mémoire pour les attributs de l'automate.
        {
          AState *state0 = malloc(sizeof(AState));
          AState *state1 = malloc(sizeof(AState));
          ATransition *transition0 = malloc(sizeof(ATransition));

          // On initialise les états et la transition.
          initialize_state(state0, index++, IS_START, NOT_FINAL);
          initialize_state(state1, index++, NOT_START, IS_FINAL);
          initialize_transition(transition0, state0, state1, tree->leaves[i]->value);

          // On pointe les transitions dans les états.
          add_transition_to_state(transition0, state0, state1);

          // Ajouter les états et transitions à l'automate.
          add_state_to_nfa(nfa, state0, state_counter++);
          add_state_to_nfa(nfa, state1, state_counter++);
          add_transition_to_nfa(nfa, transition0, transition_counter++);

          // On marque les états initiaux et finaux du sous-nfa.
          initial_node[i] = state0;
          final_node[i] = state1;
        }
        break;
    }
  }

  // On combine les états des concatenations et elimine les états intermédiaires
  merge_nodes_automaton(nfa);

  return nfa;
}