# ifndef F_NFA_H
# define F_NFA_H

#include "f_tree.h"

// Declaration avancée
struct NFAState;
struct NFATransition;
struct NFA;

// Transitions de l'automate
typedef struct NFATransition {
  char symbol;                      // Symbole de la transition
  struct NFAState *from;            // État de départ de la transition
  struct NFAState *to;              // État d'arrivée de la transition
} NFATransition;

// États de l'automate
typedef struct NFAState {
    int index;                      // Indice de l'état
    int is_start;                   // Indicateur d'état de départ
    int is_final;                   // Indicateur d'état final
    int num_transitions;            // Nombre de transitions sortantes
    NFATransition **transitions;     // Pointeur vers les états suivantes
} NFAState;

// Automate non déterministe
typedef struct NFA {
  int num_states;                   // Nombre d'états de l'automate
  int num_transitions;              // Nombre de transitions de l'automate
  NFAState **states;                 // Liste des états de l'automate
  NFATransition **transitions;       // Liste des transitions de l'automate
} NFA;

// Fonctions
NFA *create_nfa_from_syntax_tree(SyntaxTree *tree);

void initialize_nfa(NFA *return_nfa, SyntaxTree *tree);
void initialize_state(NFAState* state, int index, int is_start, int is_final);
void initialize_transition(NFATransition* transition, NFAState* from, NFAState* to, char symbol);
void add_transition_to_state(NFATransition* transition, NFAState* state);
void add_transition_to_nfa(NFA* nfa, NFATransition* transition, int counter);
void add_state_to_nfa(NFA* nfa, NFAState* state, int position, int counter);

void export_nfa_graph(NFA *nfa, char *filename);
void display_nfa(NFA *nfa);

void print_state(NFAState *state);
void print_transition(NFATransition *transition);
void print_nfa(NFA *nfa);

#endif