#include <stdio.h>
#include "display.h"
#include "board.h"

#define LIGHT_FG "38;5;15"
#define LIGHT_BG "48;5;166"
#define DARK_FG "38;5;16"
#define DARK_BG "48;5;52"

#define PRINT_UPPER_BOARD() disp(" ");\
	set_colors(WHITE, DARK);\
	disp("   \e[4m A  B  C  D  E  F  G  H \e[0m");\
	set_colors(0, DARK);\
	disp("  \n");\
	reset_colors()

#define PRINT_LOWER_BOARD() disp(" ");\
	set_colors(0, DARK);\
	disp("                             ");\
	reset_colors();\
	disp("\n")

#define PRINT_LEFT_SIDE_OF_BOARD() disp(" ");\
		set_colors(WHITE, DARK);\
		disp(" %d|", i + 1)

#define PRINT_RIGHT_SIDE_OF_BOARD() set_colors(WHITE, DARK);\
		disp("  \n");\
		reset_colors()

static char get_piece(Piece piece);
static wchar_t get_piece_unicode(Piece piece);

int show_board(void) {
	
	PRINT_UPPER_BOARD();
	for (int i = ROW_COUNT - 1; i >= _1_; i--) {
		
		PRINT_LEFT_SIDE_OF_BOARD();
		for (int j = _A_; j < COL_COUNT; j++) {
			(board[i][j].color == WHITE) ? set_colors(WHITE, 0) : set_colors(BLACK, 0);
			((i+j)%2) ? set_colors(0, LIGHT) : set_colors(0, DARK);
			
			//disp(" %lc ", board[i][j].type ? board[i][j].type : ' ');
			disp(" %lc ", get_piece_unicode(board[i][j].type));
			reset_colors();
		}
		PRINT_RIGHT_SIDE_OF_BOARD();
	}
	PRINT_LOWER_BOARD();
}

char get_piece(Piece piece) {
	
	switch (piece) {
		case PAWN: return 'P';
		case KNIGHT: return 'N';
		case BISHOP: return 'B';
		case ROOK: return 'R';
		case QUEEN: return 'Q';
		case KING: return 'K';
		case BLANK: return ' ';
		default:
			/* Error */
			return 'X';
	}
}

wchar_t get_piece_unicode(Piece piece) {
	
	switch (piece) {
		case PAWN: return 0x265F;
		case KNIGHT: return 0x265E;
		case BISHOP: return 0x265D;
		case ROOK: return 0x265C;
		case QUEEN: return 0x265B;
		case KING: return 0x265A;
		case BLANK: return ' ';
		default:
			/* Error */
			return 'X';
	}
}

void set_colors(PieceColor fg, BoardColor bg) {

	if (fg == WHITE)
        disp("\e[1;"LIGHT_FG"m");
    else if (fg == BLACK)
        disp("\e[1;"DARK_FG"m");
	
    if (bg == LIGHT)
        disp("\e[40;"LIGHT_BG"m");
    else if (bg == DARK)
        disp("\e[40;"DARK_BG"m");
}

void reset_colors() {
    disp("\e[0m");
}
#if 0
int show_board_coordinates(void) {
	disp("      A    B    C    D    E    F    G    H\n");
	
	for (int i = ROW_COUNT - 1; i >= _1_; i--) {
		disp("    -----------------------------------------\n");
		disp("  %d ", i + 1);
		for (int j = _A_; j < COL_COUNT; j++) {
			disp("| %hhu%hhu ", board[i][j].location.col, board[i][j].location.row);
		}
		disp("|\n");
	}
	disp("    -----------------------------------------\n");
}
#endif