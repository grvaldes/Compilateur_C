# ifndef F_NFA_H
# define F_NFA_H

#include "f_tree.h"

// Declaration avancée
struct AState;
struct ATransition;
struct Automaton;

// Transitions de l'automate
typedef struct NodeSet {
  int *node_array;
  int array_size;
} NodeSet;

typedef struct ATransition {
  char symbol;                    // Symbole de la transition
  struct AState *from;            // État de départ de la transition
  struct AState *to;              // État d'arrivée de la transition
} ATransition;

// États de l'automate
typedef struct AState {
    int index;                    // Indice de l'état
    int is_start;                 // Indicateur d'état de départ
    int is_final;                 // Indicateur d'état final
    int is_deleted;               // Indicateur d'état ignoré.
    int num_transitions;          // Nombre de transitions sortantes
    ATransition **transitions;    // Pointeur vers les états suivantes
    NodeSet *states_set;          // Indices des états contenus
} AState;

// Automate non déterministe
typedef struct Automaton {
  int num_states;                 // Nombre d'états de l'automate
  int num_transitions;            // Nombre de transitions de l'automate
  int num_unique_chars;
  char* unique_chars;
  AState **states;                // Liste des états de l'automate
  ATransition **transitions;      // Liste des transitions de l'automate
} Automaton;

// Fonctions
Automaton *create_nfa_from_syntax_tree(SyntaxTree *tree);

void initialize_nfa(Automaton *nfa, SyntaxTree *tree);
void initialize_state(AState *state, int index, int is_start, int is_final);
void initialize_transition(ATransition *transition, AState *from, AState *to, char symbol);
void add_transition_to_state(ATransition *transition, AState *state);
void add_transition_to_automaton(Automaton *automat, ATransition *transition, int counter);
void add_state_to_automaton(Automaton *automat, AState* state, int position, int counter);
void merge_nodes_automaton(Automaton *automat);
void reorder_indices_automaton(Automaton *automat);

void print_state(AState *state);
void print_transition(ATransition *transition);
void print_nfa(Automaton *nfa);

void export_nfa_to_graphviz(const char *filename, Automaton *nfa);
void display_nfa(Automaton *nfa);

#endif