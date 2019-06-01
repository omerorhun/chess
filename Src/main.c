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
		
		/* RegEx ifadesinin başında bir boşluk bırakmak gerekiyor (neden) */
#if NCURSES_ENABLED
		wclear(wininput);
		box(wininput, 0, 0);
		//mvwscanw(wininput, 1, 1, " %[^\n\r]", human_input);
		//wclear(winlogs);
		// box(winlogs, 0, 0);
		// line_log = 1;
		
#else
		scanf(" %[^\n\r]", human_input);
		system("clear");
#endif
		#if 1
#if NCURSES_ENABLED
		dlog(" ");
		if (line_log > 20) {
			wclear(winlogs);
			box(winlogs, 0, 0);
			line_log = 1;
		}
		
		InputStates ret_in = get_mouse_input(&mc);
#elif NOTATION_ENABLED
		ErrorCodes ret_parse = parse_notation(human_input, &mc);
#else
		ErrorCodes ret_parse = parse_human_input(human_input, &mc);
#endif
		
#if !NCURSES_ENABLED
		if (ret_parse != ERR_OK) {
			show_board();
			dlog("Wrong input format!\nFormat: <from> <to>\nExample: e2 e4\n");
			continue;
		}
#endif
		dlog("ret_in: %d", ret_in);
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
		dlog("from: %d-%d", mc.from.col, mc.from.row);
		dlog("from: %d-%d", mc.to.col, mc.to.row);
		show_current_board();
		
#if !NCURSES_ENABLED
		show_board();
		if (ret != ERR_OK) {
			dlog("Wrong move (Error %d)\n", ret);
		}
		else {
			g_turn = (g_turn == WHITE) ? BLACK : WHITE;
			dlog("turn is %d\n", g_turn);
		}
#endif

#endif
	}
	
#if NCURSES_ENABLED
	getch();
	endwin();
#endif /* NCURSES_ENABLED */
	
	return 0;
}

ErrorCodes init(void) {
	setlocale(LC_CTYPE, "");
	
#if NCURSES_ENABLED
	if(init_display() != ERR_OK)
		return ERR_NO_COLOR_SUPPORT;
	
	init_log();
	init_input();
	nc_draw_board(&winboard, BOARD_BOX_HEIGHT, BOARD_BOX_WIDTH, BOARD_BOX_Y, BOARD_BOX_X);
	init_mouse();
#endif
}

