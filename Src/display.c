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

int show_board(void) {
	
	PRINT_UPPER_BOARD();
	for (int i = ROW_COUNT - 1; i >= _1_; i--) {
		
		PRINT_LEFT_SIDE_OF_BOARD();
		for (int j = _A_; j < COL_COUNT; j++) {
			(board[i][j].color == WHITE) ? set_colors(WHITE, 0) : set_colors(BLACK, 0);
			((i+j)%2) ? set_colors(0, LIGHT) : set_colors(0, DARK);
			
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

#if NCURSES_ENABLED
ErrorCodes init_display(void) {
	
	if(has_colors())
		return ERR_NO_COLOR_SUPPORT;
	
	initscr();
	cbreak();
	noecho();
	
	curs_set(0); /* Hide the cursor */
	
	start_color();
	use_default_colors();
	
	init_color(COLOR_LIGHT, 204, 102, 0);
	init_color(COLOR_DARK, 153, 76, 0);
	init_color(COLOR_BLACK, 0, 0, 0);
	
	init_pair(DARK_WHITE, COLOR_WHITE, COLOR_DARK);
	init_pair(DARK_BLACK, COLOR_BLACK, COLOR_DARK);
	init_pair(LIGHT_WHITE, COLOR_WHITE, COLOR_LIGHT);
	init_pair(LIGHT_BLACK, COLOR_BLACK, COLOR_LIGHT);
	init_pair(WHITE_ON_DEFAULT, COLOR_WHITE, -1);
	init_pair(SELECT_DARK_WHITE, COLOR_WHITE, COLOR_BLUE);
	init_pair(SELECT_DARK_BLACK, COLOR_BLACK, COLOR_BLUE);
	init_pair(SELECT_LIGHT_WHITE, COLOR_WHITE, COLOR_BLUE);
	init_pair(SELECT_LIGHT_BLACK, COLOR_BLACK, COLOR_BLUE);
	
	return ERR_OK;
}

void nc_draw_board(WINDOW **window, int h, int w, int y, int x) {
	if (*window == NULL) {
		*window = newwin(h, w, y, x);
	}
	
	wattron(*window, COLOR_PAIR(WHITE_ON_DEFAULT));
	mvwprintw(*window, 1, 1, "   A  B  C  D  E  F  G  H ");
	box(*window, 0, 0);
	wattroff(*window, COLOR_PAIR(WHITE_ON_DEFAULT));
	
	for (int i = ROW_COUNT - 1; i >= _1_; i--) {
		wattron(*window, COLOR_PAIR(WHITE_ON_DEFAULT));
		mvwprintw(*window, i + 2, 1, "%d", 8-i);
		wattroff(*window, COLOR_PAIR(WHITE_ON_DEFAULT));
		for (int j = _A_; j < COL_COUNT; j++) {
			int colors = 0;
			
			if (board[i][j].color == WHITE)
				colors = ((i+j)%2) ? LIGHT_WHITE : DARK_WHITE;
			else
				colors = ((i+j)%2) ? LIGHT_BLACK : DARK_BLACK;
			
			wattron(*window, COLOR_PAIR(colors));
			mvwprintw(*window, 7 - i+2, 3*j+3, " %lc ", 
						get_piece_unicode(board[i][j].type));
			wattroff(*window, COLOR_PAIR(colors));
		}	
	}
	
	refresh();
	wrefresh(*window);
}

/******************************************************************************
 * @name	nc_display_move
 * @desc	Displays the move
 * @param	mc: Move struct (must be valid move)
 * @retval	None
 *****************************************************************************/
void nc_display_move(MoveCoordinates mc) {
	int colors = ((mc.from.row + mc.from.col) % 2) ? LIGHT_BLACK : DARK_BLACK;
	
	wattron(winboard, COLOR_PAIR(colors));
	mvwprintw(winboard, 7 - mc.from.row + 2, 3 * mc.from.col + 3, "   ");
	wattroff(winboard, COLOR_PAIR(colors));
	
	if (board[mc.to.row][mc.to.col].color == WHITE)
		colors = ((mc.to.row + mc.to.col) % 2) ? LIGHT_WHITE : DARK_WHITE;
	else
		colors = ((mc.to.row + mc.to.col) % 2) ? LIGHT_BLACK : DARK_BLACK;
	
	wattron(winboard, COLOR_PAIR(colors));
	mvwprintw(winboard, 7 - mc.to.row + 2, 3 * mc.to.col + 3, " %lc ", 
				get_piece_unicode(board[mc.to.row][mc.to.col].type));
	wattroff(winboard, COLOR_PAIR(colors));
	
	wrefresh(winboard);
	refresh();
}

void remove_piece(int row, int col) {
	int colors = ((row + col) % 2) ? LIGHT_BLACK : DARK_BLACK;
	
	wattron(winboard, COLOR_PAIR(colors));
	mvwprintw(winboard, 7 - row + 2, 3 * col + 3, "   ");
	wattroff(winboard, COLOR_PAIR(colors));
}

/******************************************************************************
 * @name	focus_square
 * @desc	focus on specified coordinates
 * @param	x: Column number
 * 			y: Row number (in display)
 * @retval	None
 *****************************************************************************/
void focus_square(int x, int y) {
	dlog("focus %d-%d", x, y);
	if (y > 7 || y < 0 || x > 8 || x < 0)
		return;
		
	wchar_t wch = get_piece_unicode(board[7-y][x].type);
	int colors;
	
	if (board[7-y][x].color == WHITE)
		colors = ((y + x) % 2) ? SELECT_LIGHT_WHITE : SELECT_DARK_WHITE;
	else
		colors = ((y + x) % 2) ? SELECT_LIGHT_BLACK : SELECT_DARK_BLACK;
	
	wattron(winboard, COLOR_PAIR(colors));
	mvwprintw(winboard, y + 2, 3 * x + 3, " %lc ", wch);
	wattroff(winboard, COLOR_PAIR(colors));
}

/******************************************************************************
 * @name	unfocus
 * @desc	Clear the focus of previous square
 * @param	x: Column number
 * 			y: Row number (in real)
 * @retval	None
 *****************************************************************************/
void unfocus(int x, int y) {
	dlog("unfocus %d-%d", x, y);
	if (y > 7 || y < 0 || x > 8 || x < 0)
		return;
	
	wchar_t wch = get_piece_unicode(board[y][x].type);
	int colors;
	
	if (board[y][x].color == WHITE)
		colors = ((y + x) % 2) ? LIGHT_WHITE : DARK_WHITE;
	else
		colors = ((y + x) % 2) ? LIGHT_BLACK : DARK_BLACK;

	wattron(winboard, COLOR_PAIR(colors));
	mvwprintw(winboard, 7 - y + 2, 3 * x + 3, " %lc ", wch);
	wattroff(winboard, COLOR_PAIR(colors));
}

void show_current_board(void) {
	nc_draw_board(&winboard_test, BOARD_BOX_HEIGHT, BOARD_BOX_WIDTH, BOARD_BOX_Y + 20, BOARD_BOX_X);
}

void show_valid_squares(PieceInfo pinfo) {
	
	if (pinfo.type == PAWN) {
		
		
		
		if (!pinfo.is_moved) {
			
		}
	}
	
}



#endif
