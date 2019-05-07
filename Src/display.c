#include <stdio.h>
#include "display.h"
#include "board.h"

#define LIGHT_FG "38;5;15"
#define LIGHT_BG "48;5;166"
#define DARK_FG "38;5;16"
#define DARK_BG "48;5;52"

static void reset_color();
static void set_fg_color(int x);
static void set_bg_color(int x);

static char get_piece(Piece piece);
static void show_piece(Piece piece);

int show_board(void) {
	fprintf(stdout, " ");
	set_fg_color(0);
	set_bg_color(1);
	fprintf(stdout, "   \033[4m A  B  C  D  E  F  G  H ");
	reset_color();
	set_bg_color(1);
	fprintf(stdout, "  \n");
	reset_color();
	
	for (int i = ROW_COUNT - 1; i >= _1_; i--) {
		fprintf(stdout, " ");	
		set_fg_color(0);
		set_bg_color(1);
		fprintf(stdout, " %d|", i + 1);
		
		for (int j = _A_; j < COL_COUNT; j++) {
			(board[i][j].color == WHITE) ? set_fg_color(0) : set_fg_color(1);
			((i+j)%2) ? set_bg_color(0) : set_bg_color(1);
			
			fprintf(stdout, " %c ", get_piece(board[i][j].type));
			reset_color();
		}
		set_fg_color(0);
		set_bg_color(1);
		fprintf(stdout, "  \n");
		reset_color();
	}
	fprintf(stdout, " ");
	set_bg_color(1);
	fprintf(stdout, "                             ");
	reset_color();
	fprintf(stdout, "\n");
}

int show_board_coordinates(void) {
	printf("      A    B    C    D    E    F    G    H\n");
	
	for (int i = ROW_COUNT - 1; i >= _1_; i--) {
		fprintf(stdout, "    -----------------------------------------\n");
		fprintf(stdout, "  %d ", i + 1);
		for (int j = _A_; j < COL_COUNT; j++) {
			
			//fprintf(stdout, "| %c ", get_piece(board[i][j].type));
			fprintf(stdout, "| %hhu%hhu ", board[i][j].location.col, board[i][j].location.row);
		}
		fprintf(stdout, "|\n");
	}
	fprintf(stdout, "    -----------------------------------------\n");
}

void show_piece(Piece piece) {
	switch (piece) {
		case PAWN:
			fputc('P', stdout);
			break;
		case KNIGHT:
			fputc('K', stdout);
			break;
		case BISHOP:
			fputc('B', stdout);
			break;
		case ROOK:
			fputc('R', stdout);
			break;
		case QUEEN:
			fputc('Q', stdout);
			break;
		case KING:
			fputc('K', stdout);
			break;
		case BLANK:
			fputc(' ', stdout);
			break;
		default:
			/* Error */
			fputc('X', stdout);
			break;
		
	}
}

char get_piece(Piece piece) {
	switch (piece) {
		case PAWN:
			return 'P';
		case KNIGHT:
			return 'N';
		case BISHOP:
			return 'B';
		case ROOK:
			return 'R';
		case QUEEN:
			return 'Q';
		case KING:
			return 'K';
		case BLANK:
			return ' ';
		default:
			/* Error */
			return 'X';
	}
}

void set_bg_color(int x) {
    if (x == 0)
        printf("\033[40;"LIGHT_BG"m");
    else
        printf("\033[40;"DARK_BG"m");
}

void set_fg_color(int x) {
    if (x == 0)
        printf("\033[1;"LIGHT_FG"m");
    else
        printf("\033[1;"DARK_FG"m");
}

void reset_color() {
    printf("\033[0m");
}