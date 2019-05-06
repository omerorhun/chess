#include <stdio.h>
#include "display.h"
#include "board.h"

static char get_piece(Piece piece);
static void show_piece(Piece piece);

int show_board(void) {
	printf("      A   B   C   D   E   F   G   H\n");
	
	for (int i = ROW_COUNT - 1; i >= _1_; i--) {
		fprintf(stdout, "    ---------------------------------\n");
		fprintf(stdout, "  %d ", i + 1);
		for (int j = _A_; j < COL_COUNT; j++) {
			
			fprintf(stdout, "| %c ", get_piece(board[i][j].type));
			//fprintf(stdout, "| %hhu%hhu ", board[i][j].location.col, board[i][j].location.row);
		}
		fprintf(stdout, "|\n");
	}
	fprintf(stdout, "    ---------------------------------\n");
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
