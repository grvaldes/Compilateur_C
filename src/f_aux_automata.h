# ifndef F_AUX_AUTOMATA_H
# define F_AUX_AUTOMATA_H

#include "aux_structs.h"

#define IS_FINAL 1
#define IS_START 1
#define NOT_FINAL 0
#define NOT_START 0
#define AT_START 0
#define AT_END 1

// Fonctions NFA
Automaton *create_nfa_from_syntax_tree(SyntaxTree *tree);
void initialize_nfa(Automaton *nfa, SyntaxTree *tree);
void initialize_state(AState *state, int index, int is_start, int is_final);
void initialize_transition(ATransition *transition, AState *from, AState *to, char symbol);
void add_transition_to_state(ATransition *transition, AState *state);
void add_transition_to_nfa(Automaton *automat, ATransition *transition, int counter);
void add_state_to_nfa(Automaton *automat, AState* state, int counter);
void merge_nodes_automaton(Automaton *automat);
void reorder_indices_automaton(Automaton *automat);

void print_state(AState *state);
void print_transition(ATransition *transition);
void print_nfa(Automaton *nfa);

void export_nfa_to_graphviz(const char *filename, Automaton *nfa);
void display_nfa(Automaton *nfa);


// Fonctions DFA
Automaton *create_dfa_from_nfa(Automaton *nfa);
void follow_path_from_single_state(NodeSet *node_set, AState *initial_state, char value);
void follow_path_from_group_state(Automaton *dfa, Automaton *nfa, AState *initial_state, char value);

void initialize_dfa(Automaton *dfa, Automaton *nfa);
void initialize_dfa_state(AState *state, int index, int is_start, int is_final, NodeSet *node_set);
void add_transition_to_dfa(Automaton *dfa, ATransition *transition);
void add_state_to_dfa(Automaton *nfa, AState *state);

int state_in_node_set(int index, NodeSet *node_set);
int compare_node_sets(NodeSet *set1, NodeSet *set2);
void check_final_states(Automaton *dfa, Automaton *nfa);


void export_dfa_to_graphviz(const char *filename, Automaton *dfa);
void display_dfa(Automaton *dfa);
void print_dfa(Automaton *dfa);


#endif