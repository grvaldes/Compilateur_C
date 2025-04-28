#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "f_export.h"

#define ASCII_CHARS 75
#define ASCII_START 48

void export_automate_code(Automaton *dfa, const char *expression, const char *filename) {
  FILE* f = fopen(filename, "w");
  if (!f) {
    perror("Erreur d'ouverture de fichier.");
    return;
  }

  int start_index;

  for (int i=0; i < dfa->num_states; i++) {
    if (dfa->states[i]->is_start) {
      start_index = dfa->states[i]->index;
      break;
    }
  }

  NodeSet end_set;
  end_set.array_size = 0;
  end_set.node_array = NULL;

  for (int i=0; i < dfa->num_states; i++) {
    if (dfa->states[i]->is_final) {
      end_set.array_size++;
      end_set.node_array = realloc(end_set.node_array, end_set.array_size);
      end_set.node_array[end_set.array_size-1] = dfa->states[i]->index;
      break;
    }
  }

  fprintf(f, "#include <stdio.h>\n");
  fprintf(f, "#include <stdlib.h>\n");
  fprintf(f, "#include <string.h>\n\n");
  fprintf(f, "int main(int argc, char *argv[]) {\n");
  fprintf(f, "\tint trans[%d][%d];\n", dfa->num_states, ASCII_CHARS);

  fprintf(f, "\tint end_set[%d] = { ", end_set.array_size);
  fprintf(f, "%d", end_set.node_array[0]);
  for (int i=1; i < end_set.array_size; i++) {
    fprintf(f, ", %d", end_set.node_array[i]);
  }
  fprintf(f, " };\n");

  fprintf(f, "\tint end_set_size = %d;\n", end_set.array_size);
  fprintf(f, "\tint state_index = %d;\n", start_index);
  fprintf(f, "\tint char_index = 0;\n");
  fprintf(f, "\tchar input_word[1024];\n");
  fprintf(f, "\t\n");
  fprintf(f, "\tfor (int i=0; i < %d; i++)\n", dfa->num_states);
  fprintf(f, "\t\tfor (int j=0; j < %d; j++)\n", ASCII_CHARS);
  fprintf(f, "\t\t\ttrans[i][j] = -1;\n\n");

  for (int i=0; i < dfa->num_transitions; i++) {
    int from = dfa->transitions[i]->from->index;
    int to = dfa->transitions[i]->to->index;
    int symbol = dfa->transitions[i]->symbol;

    fprintf(f, "\ttrans[%d][%d] = %d;\n", from, symbol-ASCII_START, to);
  }

  fprintf(f, "\t\n");
  fprintf(f, "\tprintf(\"Ecrivez-vous le mot a reconnaitre: \\n\");\n");
  fprintf(f, "\tscanf(\"%%s\", input_word);\n");
  fprintf(f, "\t\n");
  fprintf(f, "\twhile (input_word[char_index] != \'\\0\') {\n");
  fprintf(f, "\t\tstate_index = trans[state_index][input_word[char_index]-%d];\n", ASCII_START);
  fprintf(f, "\t\t\n");
  fprintf(f, "\t\tif (state_index == -1) {\n");
  fprintf(f, "\t\t\tprintf(\"Le mot \\\'%%s\\\' n'appartient pas au langage rationnel defini par l'expression \\\'%s\\\'\\n\", input_word);\n", expression);
  fprintf(f, "\t\t\treturn 1;\n");
  fprintf(f, "\t\t} else {\n");
  fprintf(f, "\t\t\tchar_index++;\n");
  fprintf(f, "\t\t}\n");
  fprintf(f, "\t}\n");
  fprintf(f, "\t\n");
  fprintf(f, "\tfor (int i=0; i < end_set_size; i++) {\n");
  fprintf(f, "\t\tif (state_index == end_set[i]) {\n");
  fprintf(f, "\t\t\tprintf(\"Le mot \\\'%%s\\\' appartient au langage rationnel defini par l'expression \\\'%s\\\'\\n\", input_word);\n", expression);
  fprintf(f, "\t\t\treturn 0;\n");
  fprintf(f, "\t\t}\n");
  fprintf(f, "\t}\n");
  fprintf(f, "\t\n");
  fprintf(f, "\tprintf(\"Le mot \\\'%%s\\\' n'appartient pas au langage rationnel defini par l'expression \\\'%s\\\'\\n\", input_word);\n", expression);
  fprintf(f, "\treturn 1;\n");
  fprintf(f, "}");
  
  fclose(f);
}