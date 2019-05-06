#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


#include "display.h"
#include "board.h"

#define HUMAN_INPUT_SIZE 16

PieceInfo board[ROW_COUNT][COL_COUNT] = STARTING_POSITION;

ErrorCodes parse_human_input(char *human_input, MoveCoordinates *move);

int main() {
	MoveCoordinates mc;
	char human_input[HUMAN_INPUT_SIZE];
	
	init_board();
	
	system("clear");
	
	show_board();
	
	while (1) {
		memset(human_input, 0, sizeof(human_input));
		
		/* RegEx ifadesinin başında bir boşluk bırakmak gerekiyor (neden) */
		scanf(" %[^\n\r]", human_input);
		system("clear");
		ErrorCodes ret_parse = parse_human_input(human_input, &mc);
		if (ret_parse != ERR_OK) {
			printf("Wrong input format!\nFormat: <from> <to>\nExample: e2 e4\n");
			continue;
		}
		
		ErrorCodes ret = make_move(mc);
		if (ret != ERR_OK) {
			printf("Wrong move (Error %d)\n", ret);
			continue;
		}
		
		

		show_board();
	}
	
	return 0;
}

ErrorCodes parse_human_input(char *human_input, MoveCoordinates *move) {
	int ret = sscanf(human_input, "%c%hhu %c%hhu", &move->from_c, &move->from_r, 
											&move->to_c, &move->to_r);
	
	if (ret == 4) {
		move->from_c = tolower(move->from_c - 'a');
		move->from_r = move->from_r - 1;
		move->to_c = tolower(move->to_c - 'a');
		move->to_r = move->to_r - 1;
		
		if (move->from_r < 0 || move->from_r > 7 ||
			move->from_c < 0 || move->from_c > 7 ||
			move->to_r < 0 || move->to_r > 7 ||
			move->to_c < 0 || move->to_c > 7) {
			return ERR_WRONG_INPUT_FORMAT;
		}

		return ERR_OK;
	}
	else
		return ERR_WRONG_INPUT_FORMAT;
}
