#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "board.h"

static ErrorCodes check_move_if_valid(MoveCoordinates move);
static int8_t get_col_dist(MoveCoordinates mc);
static int8_t get_row_dist(MoveCoordinates mc);
static Piece get_piece_type(uint8_t row, uint8_t col);
static PieceInfo get_piece_info(uint8_t row, uint8_t col);
static ErrorCodes check_the_route(MoveCoordinates *mc);
static ErrorCodes check_if_rook(MoveCoordinates move);
static void move_piece(MoveCoordinates *move);
static ErrorCodes check_if_enpassant(MoveCoordinates move);

PieceColor g_turn = WHITE;
Coordinates g_enpassant;

ErrorCodes make_move(MoveCoordinates move) {
	ErrorCodes ret = ERR_OK;
	
	/* Check the move */
	ret = check_move_if_valid(move);
	
	if (ret == ERR_OK) {

		/* Check extra move */
		ErrorCodes ret = check_if_rook(move);

		uint8_t row;
		row = (get_piece_info(move.from_r, move.from_c).color == WHITE) ? _1_ : _8_;
		
		if (ret == ERR_OK_ROOK_SHORT) {
			MoveCoordinates move_rook;
			move_rook.from_r = row, move_rook.from_c = _H_;
			move_rook.to_r = row, move_rook.to_c = _F_;
			move_piece(&move_rook);
		}
		else if (ret == ERR_OK_ROOK_LONG) {
			MoveCoordinates move_rook;
			move_rook.from_r = row, move_rook.from_c = _A_;
			move_rook.to_r = row, move_rook.to_c = _D_;
			move_piece(&move_rook);
		}

		/* Play the move */
		move_piece(&move);
	}
	
	return ret;
}

ErrorCodes check_move_if_valid(MoveCoordinates move) {
	PieceInfo piece = board[move.from_r][move.from_c];
	PieceInfo target = board[move.to_r][move.to_c];
	ErrorCodes ret = ERR_OK;
	
	if (piece.type == BLANK)
		return ERR_BLANK_MOVE;

	/* TODO: Hamle sırasını kontrol et */
	if (piece.color != g_turn)
		return ERR_WRONG_TURN;
	
	/* TODO: Taş açmazda mı kontrol et */
	if (piece.is_locked)
		return ERR_INVALID_MOVE_LOCKED_PIECE;

	/* TODO: İlk ve son konumun farklı olduğunu kontrol et */
	if (abs(get_row_dist(move)) + abs(get_col_dist(move)) == 0)
		return ERR_INVALID_MOVE_WRONG_COORDINATES;
	
	/* TODO: Hedef konumda bizim taşımız olmadığını kontrol et */
	if (get_piece_info(move.to_r, move.to_c).color == get_piece_info(move.from_r, move.from_c).color)
		return ERR_INVALID_MOVE_FRIENDLY_FIRE;
	
	if (piece.type == PAWN) {
		
		/* İleri gitmek zorunda */
		if ((move.to_r <= move.from_r) && (piece.color == WHITE))
			return ERR_INVALID_MOVE_PAWN_MUST_GO_FORWARD;
		else if ((move.to_r >= move.from_r) && (piece.color == BLACK))
			return ERR_INVALID_MOVE_PAWN_MUST_GO_FORWARD;
		
		if ((abs(get_col_dist(move)) == 2) && (get_row_dist(move) == 0) && !piece.is_moved) {
			/* 2 kare ileri */
			clog("en passant available\n");
			g_enpassant.row = move.to_r;
			g_enpassant.col = move.to_c;
			return ERR_OK;
		}
		else if (abs(get_col_dist(move)) == 1) {
			if (get_row_dist(move) == 0) {
				/* Tek kare ileri */
				return ERR_OK;
			}
			else if (abs(get_row_dist(move)) == 1) {
				/* Taş yeme (çapraz) */
				if (get_piece_type(move.to_r, move.to_c) != BLANK) {
					return ERR_OK;
				}
				
				/* TODO: Geçerken alma */
				/* Piyonun 5. sırada olduğunu kontrol et */
				/* Yanda piyon olduğunu ve son hamlede iki ileri sürüldüğünü kontrol et */
				ErrorCodes ret = check_if_enpassant(move);
				if (ret == ERR_OK) {
					board[g_enpassant.row][g_enpassant.col].type = BLANK;
					board[g_enpassant.row][g_enpassant.col].color = EMPTY;
				}
				
				return ret;
			}
		}

		return ERR_INVALID_MOVE;
	}
	else if (piece.type == BISHOP) {
		/* TODO: Hareket çapraz mı kontrol et */
		if (abs(get_row_dist(move)) != abs(get_col_dist(move)))
			return ERR_INVALID_MOVE_BISHOP_NOT_DIAGONAL;

		/* TEST: Hareket yolunda üzerinden atlanan taş var mı */
		return check_the_route(&move);
	}
	else if (piece.type == KNIGHT) {
		/* TODO: Hareket L şeklinde mi kontrol et */
		if (abs(get_col_dist(move) * get_row_dist(move)) != 2)
			return ERR_INVALID_MOVE_KNIGHT_NOT_L;
	}
	else if (piece.type == ROOK) {
		/* TODO: Hareket düz mü kontrol et */
		if (get_col_dist(move) * get_row_dist(move) != 0)
			return ERR_INVALID_MOVE_ROOK_NOT_STRAIGHT;
		/* TEST: Hareket yolunda üzerinden atlanan taş var mı */
		return check_the_route(&move);
	}
	else if (piece.type == QUEEN) {
		/* TODO: Hareket düz veya çapraz mı kontrol et */
		if (!((get_col_dist(move) * get_row_dist(move) == 0) ||
			(abs(get_row_dist(move)) == abs(get_col_dist(move))))) {
			return ERR_INVALID_MOVE_QUEEN_NOT_STRAIGHT_OR_DIAGONAL;
		}
		/* TEST: Hareket yolunda üzerinden atlanan taş var mı */
		return check_the_route(&move);
	}
	else if (piece.type == KING) {
		/* TODO: Çevresindeki karelere mi hareket ediyor */
		if ((abs(get_col_dist(move)) <= 1) && (abs(get_row_dist(move)) <= 1))
			return ERR_OK;
		
		/* TODO: Rook kontrolü */
		if (get_col_dist(move) == 0) {
			uint8_t row;
			row = (get_piece_info(move.from_r, move.from_c).color == WHITE) ? _1_ : _8_;
				
			PieceInfo left = get_piece_info(row, 0);
			PieceInfo right = get_piece_info(row, 7);
			uint8_t is_lrook_moved = (left.type == ROOK) ? left.is_moved : 1;
			uint8_t is_rrook_moved = (right.type == ROOK) ? right.is_moved : 1;

			if ((get_row_dist(move) == 2 && !is_rrook_moved) ||
				(get_row_dist(move) == -2 && !is_lrook_moved))
			{
				if (piece.is_moved)
					return ERR_INVALID_MOVE_ROOK;
				
				return ERR_OK;
			}
			else {
				return ERR_INVALID_MOVE_ROOK;
			}
		}
		
	}
	
	return ERR_OK;
}

/* Get distance on column */
int8_t get_col_dist(MoveCoordinates mc) {
	return mc.to_r - mc.from_r;
}

/* Get distance on row */
int8_t get_row_dist(MoveCoordinates mc) {
	return mc.to_c - mc.from_c;
}

Piece get_piece_type(uint8_t row, uint8_t col) {
	return board[row][col].type;
}

PieceInfo get_piece_info(uint8_t row, uint8_t col) {
	return board[row][col];
}

ErrorCodes check_the_route(MoveCoordinates *mc) {
	uint8_t row_t = mc->from_r;
	uint8_t col_t = mc->from_c;
	int8_t diff_r = mc->to_r - mc->from_r;
	int8_t diff_c = mc->to_c - mc->from_c;

	row_t += (diff_r == 0) ? 0 : (diff_r < 0) ? -1 : 1;
	col_t += (diff_c == 0) ? 0 : (diff_c < 0) ? -1 : 1;

	/* TODO: Burada iki değişkeni de kontrol etmeye gerek yok aslında */
	while (!((row_t == mc->to_r) && (col_t == mc->to_c))) {
		
		if (get_piece_type(row_t, col_t) != BLANK)
			return ERR_INVALID_MOVE_NOT_CLEAR_ROUTE;

		row_t += (diff_r == 0) ? 0 : (diff_r < 0) ? -1 : 1;
		col_t += (diff_c == 0) ? 0 : (diff_c < 0) ? -1 : 1;
	}

	return ERR_OK;
}

ErrorCodes check_if_rook(MoveCoordinates move) {
	PieceInfo piece = get_piece_info(move.from_r, move.from_c);

	if (piece.is_moved)
		return ERR_INVALID_MOVE_ROOK;
	
	/* TODO: Yol üzerinde taş var mı kontrol et */
	/* TODO: Yolu gören rakip taş var mı kontrol et */

	if (get_col_dist(move) == 0) {
		uint8_t row = (piece.color == WHITE)? _1_ : _8_;
		PieceInfo left = get_piece_info(row, _A_);
		PieceInfo right = get_piece_info(row, _H_);
		uint8_t is_lrook_moved = (left.type == ROOK) ? left.is_moved : 1;
		uint8_t is_rrook_moved = (right.type == ROOK) ? right.is_moved : 1;

		if (get_row_dist(move) == 2 && !is_rrook_moved) {
			return ERR_OK_ROOK_SHORT;
		}
		else if (get_row_dist(move) == -2 && !is_lrook_moved) {
			return ERR_OK_ROOK_LONG;
		}
		else {
			return ERR_INVALID_MOVE_ROOK;
		}
	}
}

ErrorCodes check_if_enpassant(MoveCoordinates move) {
	
	if (abs(get_row_dist(move) == 1) && (move.to_c == g_enpassant.col)) {
		return ERR_OK;
	}

	return ERR_NOT_ENPASSANT_MOVE;
}

void move_piece(MoveCoordinates *move) {
	board[move->to_r][move->to_c].type = board[move->from_r][move->from_c].type;
	board[move->to_r][move->to_c].color = board[move->from_r][move->from_c].color;
	board[move->to_r][move->to_c].is_moved = 1;
	board[move->from_r][move->from_c].type = BLANK;
	board[move->from_r][move->from_c].color = EMPTY;
}

