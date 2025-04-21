# ifndef F_DFA_H
# define F_DFA_H

#include "f_tree.h"
#include "f_nfa.h"

// Fonctions
Automaton *create_dfa_from_nfa(Automaton *nfa);
void *follow_automaton_path(NodeSet *node_set, AState *state, char value);

void initialize_dfa(Automaton *dfa, Automaton *nfa);
void initialize_dfa_state(AState *state, int index, int is_start, int is_final, NodeSet *node_set);
void add_transition_to_dfa(Automaton *dfa, ATransition *transition, int counter);
void add_state_to_dfa(Automaton *nfa, AState *state, int position, int counter);

void print_dfa(Automaton *dfa);

int state_in_node_set(int index, int *node_set);

void export_dfa_to_graphviz(const char *filename, Automaton *dfa);
void display_dfa(Automaton *dfa);

#endif