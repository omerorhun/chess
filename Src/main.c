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

#if NCURSES_ENABLED
#include <ncurses.h>
#endif /* NCURSES_ENABLED */

#define HUMAN_INPUT_SIZE 16

PieceInfo board[ROW_COUNT][COL_COUNT] = STARTING_POSITION;
//PieceInfo board[ROW_COUNT][COL_COUNT] = TEST_POSITION_ROOK_9;

ErrorCodes parse_human_input(char *human_input, MoveCoordinates *move);

extern PieceColor g_turn;
extern Coordinates g_enpassant;

ErrorCodes parse_notation(char *notation, MoveCoordinates *move);
void filter_notation(char *notation);
static void shiftl(uint8_t index, char *buffer);
ErrorCodes init(void);

int main() {
	MoveCoordinates mc;
	char human_input[HUMAN_INPUT_SIZE];
	
	/* TODO: Bunun için düzenleme yap ya da açıklama yaz */
	mc.from.row = 8;
	
	if(init() != ERR_OK) {
		printf("Terminal doesn't support colors\n");
		return 0;
	}
	
#if !NCURSES_ENABLED
	system("clear");
	set_colors(WHITE, 0);
	log(" Version: %s\n\n", VER_FILEVERSION_STR);
	show_board();
#endif
	
	while (1) {
		memset(human_input, 0, sizeof(human_input));
		
#if NCURSES_ENABLED
		// wclear(wininput);
		// box(wininput, 0, 0);
		// mvwscanw(wininput, 1, 1, " %[^\n\r]", human_input);
		// wclear(winlogs);
		// box(winlogs, 0, 0);
		// line_log = 1;
		
		dlog(" ");
		// if (line_log > 20) {
		// 	wclear(winlogs);
		// 	box(winlogs, 0, 0);
		// 	line_log = 1;
		// }
		
		InputStates ret_in = get_mouse_input(&mc);
		
		dlog("ret_mouse_in: %d\n", ret_in);
		
		if (ret_in == INPUT_READY) {
			ErrorCodes ret = ERR_OK;
			ret = make_move(mc);
			
			if (ret != ERR_OK) {
				dlog("Wrong move (Error %d)\n", ret);
				unfocus(mc.from.col, mc.from.row);
				refresh_move(&mc);
			}
			else {
				nc_display_move(mc);
				memset(&mc, 0, sizeof(MoveCoordinates));
				mc.from.row = 8;
				g_turn = (g_turn == WHITE) ? BLACK : WHITE;
			}
		}
		else if (ret_in == INPUT_HISTORY_MOVE) {
			g_turn = (g_turn == WHITE) ? BLACK : WHITE;
		}
		
		show_current_board();
	}
	
	getch();
	endwin();
	
#else
		/* RegEx ifadesinin başında bir boşluk bırakmak gerekiyor (neden) */
		scanf(" %[^\n\r]", human_input);
		system("clear");
		
#if NOTATION_ENABLED
		ErrorCodes ret_in = parse_notation(human_input, &mc);
		if (ret_in != ERR_OK) {
			show_board();
			dlog("Wrong input format!\nFormat: <from> <to>\nExample: e2 e4\n");
			continue;
		}
#else
		ErrorCodes ret_in = parse_human_input(human_input, &mc);
		if (ret_in != ERR_OK) {
			show_board();
			dlog("Wrong input format!\nFormat: <from> <to>\nExample: e2 e4\n");
			continue;
		}
#endif /* NOTATION_ENABLED */
		
		ErrorCodes ret = make_move(mc);
		
		if (ret != ERR_OK) {
			dlog("Wrong move (Error %d)\n", ret);
		}
		else {
			memset(&mc, 0, sizeof(MoveCoordinates));
			g_turn = (g_turn == WHITE) ? BLACK : WHITE;
		}
		
		show_board();
	}
#endif /* NCURSES_ENABLED */
	
	return 0;
}

ErrorCodes init(void) {
	setlocale(LC_CTYPE, "");
	
#if NCURSES_ENABLED
	if(init_display() != ERR_OK)
		return ERR_NO_COLOR_SUPPORT;
	
	init_log();
	//init_input();
	nc_draw_board(&winboard, BOARD_BOX_HEIGHT, BOARD_BOX_WIDTH, BOARD_BOX_Y, BOARD_BOX_X);
	init_mouse();
#endif
	return ERR_OK;
}

