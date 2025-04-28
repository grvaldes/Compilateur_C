#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	int trans[2][75];
	int end_set[1] = { 0 };
	int end_set_size = 1;
	int state_index = 0;
	int char_index = 0;
	char input_word[1024];
	
	for (int i=0; i < 2; i++)
		for (int j=0; j < 75; j++)
			trans[i][j] = -1;

	trans[0][51] = 0;
	trans[0][50] = 0;
	
	printf("Ecrivez-vous le mot a reconnaitre: \n");
	scanf("%s", input_word);
	
	while (input_word[char_index] != '\0') {
		state_index = trans[state_index][input_word[char_index]-48];
		
		if (state_index == -1) {
			printf("Le mot \'%s\' n'appartient pas au langage rationnel defini par l'expression \'(c|b)*\'\n", input_word);
			return 1;
		} else {
			char_index++;
		}
	}
	
	for (int i=0; i < end_set_size; i++) {
		if (state_index == end_set[i]) {
			printf("Le mot \'%s\' appartient au langage rationnel defini par l'expression \'(c|b)*\'\n", input_word);
			return 0;
		}
	}
	
	printf("Le mot \'%s\' n'appartient pas au langage rationnel defini par l'expression \'(c|b)*\'\n", input_word);
	return 1;
}