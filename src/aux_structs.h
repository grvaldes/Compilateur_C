# ifndef AUX_STRUCTS_H
# define AUX_STRUCTS_H

// Declaration avancée
struct AState;
struct ATransition;
struct Automaton;

// Noeud de l'arbre syntaxique
typedef struct TreeNode {
  char value;                     // Caractère de l'arbre
  int index;                      // Index du noeud dans l'arbre
  struct TreeNode *parent;        // Pointeur vers le parent.
  struct TreeNode *left_child;    // Pointeur vers le fils gauche ou unique.
  struct TreeNode *right_child;   // Pointeur vers fils droite ou NULL
} TreeNode;

// Arbre syntaxique
typedef struct SyntaxTree {
  int num_leaves;                 // Nombre de feuilles dans l'arbre
  TreeNode *root;                 // Pointeur vers la racine de l'arbre
  TreeNode **leaves;              // Tableau de pointeurs vers les feuilles (ordonnées).
  int num_unique_chars;
  char* unique_chars;
} SyntaxTree;

// Groupe d'états de l'automate
typedef struct NodeSet {
  int *node_array;
  int array_size;
} NodeSet;

// Transitions de l'automate
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

#endif