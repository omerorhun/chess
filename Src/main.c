#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


#include "display.h"
#include "board.h"
#include "locale.h"

#include "version.h"

#include "test.h"

#define HUMAN_INPUT_SIZE 16

#define NOTATION_ENABLED 1

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

ErrorCodes parse_notation(char *notation, MoveCoordinates *move) {

	/* TODO: x, +, #, ! karakterleri varsa kırp */
	filter_notation(notation);
	int len = strlen(notation);
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

		int8_t back = g_turn == WHITE ? 1 : -1;
		
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
		if (piece == KNIGHT) {
			
		}
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