#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "board.h"
#include "display.h"

#include "input_methods.h"
#include "moves.h"

#define INPUT_BOX_Y 1
#define INPUT_BOX_X 30
#define INPUT_BOX_WIDTH 10
#define INPUT_BOX_HEIGHT 3

typedef enum {
	INVALID_MOVE,
	PAWN_MOVE_1,
	PAWN_MOVE_2,
	NBRQK_MOVE_1,
	NBRQK_MOVE_2,
	ROOK_MOVE
}MoveType;

extern MoveTurn g_turn;

static void filter_notation(char *notation);
void shiftl(uint8_t index, char *buffer);

ErrorCodes parse_notation(char *notation, MoveCoordinates *move) {
	
	/* TODO: x, +, #, ! karakterleri varsa kırp */
	filter_notation(notation);
	
	char ch1 = 0, ch2 = 0, ch3 = 0, ch4 = 0;
	MoveType mt = INVALID_MOVE;
	
	//scanf("%[^\n]", notation);
    /* TODO: Burayı düzenle */
	if (sscanf(notation, "%1[a-h]%1[a-h]%1[1-8]\n", &ch1, &ch2, &ch3) != 3) {
    	
		if (sscanf(notation, "%1[a-h]%1[1-8]\n", &ch1, &ch2) != 2) {
            
            if (sscanf(notation, "%1[NBRQK]%1[a-h]%1[1-8]\n", &ch1, &ch2, &ch3) != 3) {
                printf("error\n");
                
                if (sscanf(notation, "%2[a-h1-8]%1[NBRQK]%1[a-h]%1[1-8]\n", &ch1, &ch2, &ch3, &ch4) != 4) {
					
				}
                else {
					mt = NBRQK_MOVE_2;
				}
            }
			else {
				mt = NBRQK_MOVE_1;
			}
        }
		else {
			mt = PAWN_MOVE_1;
		}
    }
    else {
        mt = PAWN_MOVE_2;
    }
	
	/* TODO: Rook hamlesini kontrol et */
	/* TODO: Notasyon uzunluğuna göre hangi taş ile oynanabileceğini sınırla */
	/* TODO: Piyon ise ona göre oynanacak taşın koordinatını belirle ve geri dön */
	
	dlog("mt: %d\n", mt);
	
	if (mt == PAWN_MOVE_1) {
		/* TODO: Piyon sürme */
		move->to.col = ch1 - 'a';
		move->to.row = ch2 - '0' - 1;
		dlog("%c%c\n", ch1, ch2);
		dlog("%d-%d\n", move->to.col, move->to.row);
		
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
	else if (mt == PAWN_MOVE_2) {
		/* TODO: Piyon ile taş yeme */
		move->from.col = ch1 - 'a';
		move->to.col = ch2 - 'a';
		move->to.row = ch3 - '0' - 1;
		
		dlog("pawn move 2\n");
		dlog("%c%c%c\n", ch1, ch2, ch3);
		
		if (abs(move->to.col - move->from.col) != 1) {
			return ERR_INVALID_MOVE;
		}
		int8_t back = (g_turn == WHITE) ? 1 : -1;
		
		if ((move->to.col < move->from.col) &&
			(board[move->to.row - 1 * back][move->from.col].type == PAWN) &&
			(board[move->to.row - 1 * back][move->from.col].color == g_turn))
		{
			move->from.row = move->to.row - 1 * back;
			return ERR_OK;
		}
		
		return ERR_INVALID_MOVE;
	}
	else if(mt == NBRQK_MOVE_1) {
		Piece piece = BLANK;
		
		move->to.row = ch3 - '0' - 1;
		move->to.col = ch2 - 'a';
		
		switch (ch1) {
			case 'N': piece = KNIGHT; break; /* At */
			case 'B': piece = BISHOP; break; /* Fil */
			case 'R': piece = ROOK; break; /* Kale */
			case 'Q': piece = QUEEN; break; /* Vezir */
			case 'K': piece = KING; break; /* Şah */
			default: piece = BLANK; break; /* Geçersiz */
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
	else if (mt == NBRQK_MOVE_2) {
		Piece piece = BLANK;
		
		move->to.row = ch4 - '0' - 1;
		move->to.col = ch3 - 'a';
		
		switch (ch1) {
			case 'N': piece = KNIGHT; break; /* At */
			case 'B': piece = BISHOP; break; /* Fil */
			case 'R': piece = ROOK; break; /* Kale */
			case 'Q': piece = QUEEN; break; /* Vezir */
			case 'K': piece = KING; break; /* Şah */
			default: piece = BLANK; break; /* Geçersiz */
		}
		
		/* TODO: Satır veya sütunu tara ve taşı bul */
		if (ch1 >= 'a') {
			move->from.row = ch1 - '0' - 1;
			/* Satırda tara */
			int count = 0;
			for (int i = 0; i < _H_; i++) {
				if ((board[move->from.row][i].type == piece) &&
					(board[move->from.row][i].color == g_turn))
				{
					if (++count > 1) {
						return ERR_INVALID_MOVE;
					}
					
					move->from.col = i;
				}
			}
		}
		else {
			move->from.col = ch1 - 'a';
			/* Sütunda tara */
			int count = 0;
			for (int i = 0; i < _8_; i++) {
				if ((board[i][move->from.col].type == piece) &&
					(board[i][move->from.col].color == g_turn))
				{
					if (++count > 1) {
						return ERR_INVALID_MOVE;
					}
					
					move->from.row = i;
				}
			}
		}
		
		return ERR_OK;
	}
	
	return ERR_INVALID_MOVE;
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

#if NCURSES_ENABLED
void init_input(void) {
	
	/* Init input box */
	wininput = newwin(INPUT_BOX_HEIGHT, INPUT_BOX_WIDTH, INPUT_BOX_Y, INPUT_BOX_X);
	line_in = 1;
	
	wattron(wininput, COLOR_PAIR(WHITE_ON_DEFAULT));
	box(wininput, 0, 0);
	
	refresh();
	wrefresh(wininput);
}

void init_mouse(void) {
	mvprintw(20, 40, "test string");
	refresh();
	/* Init mouse */
	mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    keypad(winboard, TRUE);
}

InputStates get_mouse_input(MoveCoordinates *mc) {
	InputStates ret = INPUT_WAITING_FIRST_COORDINATE;
	MEVENT mevt;
	static int step = 0;
	int ch = wgetch(winboard);
	
	switch (ch) {
		case KEY_MOUSE:
			if (getmouse(&mevt) == OK) {
				int xt = (mevt.x - 2)/3 - 1;
				int yt = mevt.y - 1 - 2;
				
				dlog("mx-my: %d-%d", mevt.x - 2, mevt.y - 1);
				dlog("mx-my: %d-%d", ((mevt.x - 2)/3)*3 + 1, (mevt.y - 1));
				dlog("mx-my: %d-%d", ((mevt.x - 2)/3) - 1, (mevt.y - 1) - 2);
				dlog("xt-yt: %d-%d", xt ,7-yt);
				dlog("type: %d", board[7-yt][xt].type);
				
				if (yt > 7 || yt < 0 || xt > 8 || xt < 0) {
					unfocus(mc->from.col, mc->from.row);
					refresh_move(mc);
					return INPUT_WAITING_FIRST_COORDINATE;
				}
				
				if (mevt.bstate & BUTTON1_CLICKED) {
					wchar_t wch = get_piece_unicode(board[7-yt][xt].type);
					dlog("wch: %lc", wch);
					focus_square(xt, yt);
					
					if (mc->from.row == 8) {
						dlog("case 0");
						if ((wch >= 0x265A) && (wch <= 0x265F)) {
							/* Determine 'from' coordinates */
							mc->from.row = 7 - yt;
							mc->from.col = xt;
							step++;
							ret = INPUT_WAITING_SECOND_COORDINATE;
						}
					}
					else {
						dlog("case 1");
						/* Determine 'to' coordinates */
						mc->to.row = 7 - yt;
						mc->to.col = xt;
						step = 0;
						ret = INPUT_READY;
					}
					
					
				}
			}
			break;
			
		default:
			break;
	}
	
	return ret;
}

void refresh_move(MoveCoordinates *mc) {
	if (mc->to.row > 7 || mc->to.col < 0 || mc->from.row > 8 || mc->from.col < 0)
		return;
	mc->from.row = mc->to.row;
	mc->from.col = mc->to.col;
	mc->to.row = 0;
	mc->to.col = 0;
}

#endif
