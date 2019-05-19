#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "board.h"

#include "input_methods.h"
#include "moves.h"

extern MoveTurn g_turn;

static void filter_notation(char *notation);
void shiftl(uint8_t index, char *buffer);

ErrorCodes parse_notation(char *notation, MoveCoordinates *move) {

	/* TODO: x, +, #, ! karakterleri varsa kırp */
	filter_notation(notation);
	int len = strlen(notation);
	
	printf("len: %d\n", len);
	/* TODO: Rook hamlesini kontrol et */

	/* TODO: Notasyon uzunluğuna göre hangi taş ile oynanabileceğini sınırla */
	/* TODO: Piyon ise ona göre oynanacak taşın koordinatını belirle ve geri dön */
	if (len == 2) {
		dlog("pawn move\n");
		
		sscanf(notation, "%c%hhu", &move->to.col, &move->to.row);
		move->to.col = move->to.col - 'a';
		move->to.row--;
		if (!((move->to.col <= _H_) && (move->to.col >= _A_) &&
			(move->to.row <= _8_) && (move->to.row >= _1_)))
		{
			return ERR_WRONG_NOTATION;
		}

		int8_t back = (g_turn == WHITE) ? 1 : -1;
		
		if (board[move->to.row - 1 * back][move->to.col].type == PAWN) {
			move->from.row = move->to.row - 1 * back;
			move->from.col = move->to.col;
		}
		else if (board[move->to.row - 2 * back][move->to.col].type == PAWN) {
			move->from.row = move->to.row - 2 * back;
			move->from.col = move->to.col;
		}
		else {
			return ERR_WRONG_INPUT_FORMAT;
		}
		
		return ERR_OK;
	}
	else if(len == 3) {
		dlog("len:  3\n");
		Piece piece = BLANK;
		sscanf(notation, "%*c%c%hhu",  &move->to.col, &move->to.row);
		move->to.col = move->to.col - 'a';
		move->to.row--;
		if (!((move->to.col <= _H_) && (move->to.col >= _A_) &&
			(move->to.row <= _8_) && (move->to.row >= _1_)))
		{
			return ERR_WRONG_NOTATION;
		}

		/* TODO: Hangi taş ile oyananacağını belirle */
		if (*(notation) >= 'a' && *notation <= 'h') {
			/* Piyon - taş yeme */
			piece = PAWN;
		}
		else {
			switch (*notation) {
				case 'N': piece = KNIGHT; break; /* At */
				case 'B': piece = BISHOP; break; /* Fil */
				case 'R': piece = ROOK; break; /* Kale */
				case 'Q': piece = QUEEN; break; /* Vezir */
				case 'K': piece = KING; break; /* Şah */
				default: piece = BLANK; break; /* Geçersiz */
			}
		}

		/* TODO: Belirlenen taşın hareketine göre hedefi merkez alarak tara.
		 * Eğer oynayabilecek sadece bir taş varsa ERR_OK dön
		 */
		move->from = find_starting_point(move->to, piece);
		
		if (move->from.col == COL_COUNT) {
			dlog("col_count error\n");
			return ERR_INVALID_MOVE;
		}
		
		return ERR_OK;
	}
	

	/* TODO: */
}

void filter_notation(char *notation) {
	char expected[] = "abcdefgh12345678KQNBR\0";
	
	dlog("filtering...\n");

	dlog("%s\n", notation);

	for (int i = 0; notation[i] != '\0'; i++) {
		int ctl = 0;
		for (int j = 0; expected[j] != '\0'; j++) {
			if (*(notation + i) == expected[j]) {
				ctl++;
			}
		}

		if (!ctl) {
			shiftl(i--, notation);
		}
	}

	dlog("notation: %s\n", notation);
	dlog("end filter\n");
}

void shiftl(uint8_t index, char *buffer) {
	
	int len = strlen(buffer) - index;
	for (int i = 0; i < len; i++) {
		*(buffer + index + i) = *(buffer + index + i + 1);
	}
}


ErrorCodes parse_human_input(char *human_input, MoveCoordinates *move) {
	int ret = sscanf(human_input, "%c%hhu %c%hhu", &move->from.col, &move->from.row, 
											&move->to.col, &move->to.row);
	
	if (ret == 4) {
		move->from.col = tolower(move->from.col - 'a');
		move->from.row = move->from.row - 1;
		move->to.col = tolower(move->to.col - 'a');
		move->to.row = move->to.row - 1;
		
		if (move->from.row < 0 || move->from.row > 7 ||
			move->from.col < 0 || move->from.col > 7 ||
			move->to.row < 0 || move->to.row > 7 ||
			move->to.col < 0 || move->to.col > 7) {
			return ERR_WRONG_INPUT_FORMAT;
		}

		return ERR_OK;
	}
	else
		return ERR_WRONG_INPUT_FORMAT;
}