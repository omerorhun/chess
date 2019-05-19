#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


#include "display.h"
#include "board.h"
#include "locale.h"
#include "input_methods.h"

#include "version.h"
#include "conf.h"

#include "test.h"

#define HUMAN_INPUT_SIZE 16

PieceInfo board[ROW_COUNT][COL_COUNT] = STARTING_POSITION;
//PieceInfo board[ROW_COUNT][COL_COUNT] = TEST_POSITION_ROOK_9;

ErrorCodes parse_human_input(char *human_input, MoveCoordinates *move);

extern PieceColor g_turn;
extern Coordinates g_enpassant;

ErrorCodes parse_notation(char *notation, MoveCoordinates *move);
void filter_notation(char *notation);
static void shiftl(uint8_t index, char *buffer);

int main() {
	MoveCoordinates mc;
	char human_input[HUMAN_INPUT_SIZE];
	
	setlocale(LC_CTYPE, "");
	system("clear");
	set_colors(WHITE, 0);
	log(" Version: %s\n\n", VER_FILEVERSION_STR);
	show_board();
	
	while (1) {
		memset(human_input, 0, sizeof(human_input));
		
		/* RegEx ifadesinin başında bir boşluk bırakmak gerekiyor (neden) */
		scanf(" %[^\n\r]", human_input);
		system("clear");

#if NOTATION_ENABLED
		ErrorCodes ret_parse = parse_notation(human_input, &mc);
#else
		ErrorCodes ret_parse = parse_human_input(human_input, &mc);
#endif
		if (ret_parse != ERR_OK) {
			show_board();
			dlog("Wrong input format!\nFormat: <from> <to>\nExample: e2 e4\n");
			continue;
		}
		dlog("enpassant: %hhu-%hhu\n", g_enpassant.row, g_enpassant.col);

		ErrorCodes ret = make_move(mc);
		show_board();
		if (ret != ERR_OK) {
			dlog("Wrong move (Error %d)\n", ret);
		}
		else {
			g_turn = (g_turn == WHITE) ? BLACK : WHITE;
			dlog("turn is %d\n", g_turn);
		}	
	}
	
	return 0;
}
