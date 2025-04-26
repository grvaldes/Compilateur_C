# ifndef F_AUX_AUTOMATA_H
# define F_AUX_AUTOMATA_H

#include "aux_structs.h"

#define IS_FINAL 1
#define IS_START 1
#define NOT_FINAL 0
#define NOT_START 0
#define AT_START 0
#define AT_END 1

// Fonctions Automates
void initialize_state(AState *state, int index, int is_start, int is_final);
void initialize_transition(ATransition *transition, AState *from, AState *to, char symbol);
void add_transition_to_state(ATransition *transition, AState *state);
void merge_nodes_automaton(Automaton *automat);
void export_automaton_to_graphviz(const char *filename, Automaton *automat);


// Fonctions NFA
void initialize_nfa(Automaton *nfa, SyntaxTree *tree);
void add_transition_to_nfa(Automaton *automat, ATransition *transition, int counter);
void add_state_to_nfa(Automaton *automat, AState* state, int counter, int position);


// Fonctions DFA
void follow_path_from_single_state(NodeSet *node_set, AState *initial_state, char value);
void follow_path_from_group_state(Automaton *dfa, Automaton *nfa, AState *initial_state, char value);

void initialize_dfa(Automaton *dfa, Automaton *nfa);
void initialize_dfa_state(AState *state, int index, int is_start, int is_final, NodeSet *node_set);
void add_transition_to_dfa(Automaton *dfa, ATransition *transition);
void add_state_to_dfa(Automaton *nfa, AState *state);

int state_in_node_set(int index, NodeSet *node_set);
int compare_node_sets(NodeSet *set1, NodeSet *set2);
void check_final_states(Automaton *dfa, Automaton *nfa);


// Fonctions DFA minimal
void remove_states_from_set(NodeSet *original_set, NodeSet *new_set);

#endif