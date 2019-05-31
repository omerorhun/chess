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

static uint8_t is_square_safe(Coordinates square, MoveTurn turn);

MoveTurn g_turn = WHITE;
Coordinates g_enpassant;
ExtraMove g_extra_move;

/* TODO: Oyunculara ait taşları belirten struct oluştur hamleleri oradan kontrol et */

ErrorCodes make_move(MoveCoordinates move) {
	ErrorCodes ret = ERR_OK;
	
	/* Check the move */
	ret = check_move_if_valid(move);
	
	if (ret == ERR_OK) {

		/* Check extra move */
		if (g_extra_move == SHORT_ROOK) {
			MoveCoordinates move_rook = {move.from.row, _H_, move.to.row, _F_};
			move_piece(&move_rook);
		}
		else if(g_extra_move == LONG_ROOK) {
			MoveCoordinates move_rook = {move.from.row, _A_, move.to.row, _D_};
			move_piece(&move_rook);
		}
		else if (g_extra_move == ENPASSANT) {
			board[g_enpassant.row][g_enpassant.col].type = BLANK;
			board[g_enpassant.row][g_enpassant.col].color = EMPTY;
			memset(&g_enpassant, 0, sizeof(Coordinates));
		}
		
		g_extra_move = NO_EXTRA_MOVE;

		/* Play the move */
		move_piece(&move);
	}
	
	return ret;
}

ErrorCodes check_move_if_valid(MoveCoordinates move) {
	PieceInfo piece = board[move.from.row][move.from.col];
	PieceInfo target = board[move.to.row][move.to.col];
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
	if (get_piece_info(move.to.row, move.to.col).color == get_piece_info(move.from.row, move.from.col).color)
		return ERR_INVALID_MOVE_FRIENDLY_FIRE;
	
	if (piece.type == PAWN) {
		
		/* İleri gitmek zorunda */
		if ((move.to.row <= move.from.row) && (piece.color == WHITE))
			return ERR_INVALID_MOVE_PAWN_MUST_GO_FORWARD;
		else if ((move.to.row >= move.from.row) && (piece.color == BLACK))
			return ERR_INVALID_MOVE_PAWN_MUST_GO_FORWARD;
		
		if ((abs(get_col_dist(move)) == 2) && 
			(get_row_dist(move) == 0) && 
			!piece.is_moved && 
			(get_piece_info(move.to.row, move.to.col).type == BLANK))
		{
			/* 2 kare ileri */
			dlog("en passant available\n");
			g_enpassant.row = move.to.row;
			g_enpassant.col = move.to.col;
			return ERR_OK;
		}
		else if (abs(get_col_dist(move)) == 1) {
			if ((get_row_dist(move) == 0) && 
				(get_piece_info(move.to.row, move.to.col).type == BLANK))
			{
				/* Tek kare ileri */
				return ERR_OK;
			}
			else if (abs(get_row_dist(move)) == 1) {
				/* Taş yeme (çapraz) */
				if (get_piece_type(move.to.row, move.to.col) != BLANK) {
					return ERR_OK;
				}
				
				/* TODO: Geçerken alma */
				/* Yanda piyon olduğunu ve son hamlede iki ileri sürüldüğünü kontrol et */
				if (check_if_enpassant(move) == ERR_OK) {
					g_extra_move = ENPASSANT;
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
		return check_if_rook(move);
	}
	
	return ERR_OK;
}

/* Get distance on column */
int8_t get_col_dist(MoveCoordinates mc) {
	return mc.to.row - mc.from.row;
}

/* Get distance on row */
int8_t get_row_dist(MoveCoordinates mc) {
	return mc.to.col - mc.from.col;
}

Piece get_piece_type(uint8_t row, uint8_t col) {
	return board[row][col].type;
}

PieceInfo get_piece_info(uint8_t row, uint8_t col) {
	return board[row][col];
}

ErrorCodes check_the_route(MoveCoordinates *mc) {
	uint8_t row_t = mc->from.row;
	uint8_t col_t = mc->from.col;
	int8_t diff_r = mc->to.row - mc->from.row;
	int8_t diff_c = mc->to.col - mc->from.col;

	row_t += (diff_r == 0) ? 0 : (diff_r < 0) ? -1 : 1;
	col_t += (diff_c == 0) ? 0 : (diff_c < 0) ? -1 : 1;

	/* TODO: Burada iki değişkeni de kontrol etmeye gerek yok aslında */
	while (!((row_t == mc->to.row) && (col_t == mc->to.col))) {
		
		if (get_piece_type(row_t, col_t) != BLANK) {
			dlog("error: %d-%d\n", row_t, col_t);
			dlog("type: %x\n", get_piece_type(row_t, col_t));
			return ERR_INVALID_MOVE_NOT_CLEAR_ROUTE;
		}
			

		row_t += (diff_r == 0) ? 0 : (diff_r < 0) ? -1 : 1;
		col_t += (diff_c == 0) ? 0 : (diff_c < 0) ? -1 : 1;
	}

	return ERR_OK;
}

ErrorCodes check_if_rook(MoveCoordinates move) {
	PieceInfo piece = get_piece_info(move.from.row, move.from.col);

	if (piece.is_moved) {
		printf("piece is moved\n");
		return ERR_INVALID_MOVE_ROOK;
	}
	
	if (!is_square_safe(piece.location, g_turn))
		return ERR_INVALID_MOVE_ROOK_KING_UNDERATTACK;
	
	if (get_col_dist(move) == 0) {
		if ((get_row_dist(move) == 2) && !(board[move.from.row][_H_].is_moved)) {
			/* TODO: Yol üzerinde taş var mı kontrol et */
			MoveCoordinates way = {{move.from.row, move.from.col}, {move.to.row, _H_}};
			
			 dlog("checking route %d\n", __LINE__);
			 dlog("%d-%d to %d-%d\n", way.from.row, way.from.col, way.to.row, way.to.col);

			if (check_the_route(&way) != ERR_OK)
				return ERR_INVALID_MOVE_NOT_CLEAR_ROUTE;

			/* TODO: Yolu gören rakip taş var mı kontrol et */
			for (int i = move.from.col + 1; i <= move.to.col; i++) {
				Coordinates coor = {move.from.row, i};
				if (!is_square_safe(coor, g_turn))
					return ERR_INVALID_MOVE_ROOK_KINGS_ROUTE_NOT_SAFE;
			}

			g_extra_move = SHORT_ROOK;

			return ERR_OK;
		}
		else if(get_row_dist(move) == -2 && !(board[move.from.row][_A_].is_moved)) {
			/* TODO: Yol üzerinde taş var mı kontrol et */
			MoveCoordinates way = {{move.from.row, move.from.col}, {move.to.row, _A_}};
			
			 dlog("checking route %d\n", __LINE__);
			 dlog("%d-%d to %d-%d\n", way.from.row, way.from.col, way.to.row, way.to.col);
			
			if (check_the_route(&way) != ERR_OK)
				return ERR_INVALID_MOVE_NOT_CLEAR_ROUTE;

			/* TODO: Yolu gören rakip taş var mı kontrol et */
			for (int i = move.from.col - 1; i >= move.to.col; i--) {
				Coordinates coor = {move.from.row, i};
				if (!is_square_safe(coor, g_turn))
					return ERR_INVALID_MOVE_ROOK_KINGS_ROUTE_NOT_SAFE;
			}

			g_extra_move = LONG_ROOK;

			return ERR_OK;
		}

		return ERR_INVALID_MOVE_ROOK;
	}
	
	
}

ErrorCodes check_if_enpassant(MoveCoordinates move) {
	
	if (abs(get_row_dist(move) == 1) && (move.to.col == g_enpassant.col)) {
		return ERR_OK;
	}

	return ERR_NOT_ENPASSANT_MOVE;
}

void move_piece(MoveCoordinates *move) {
	board[move->to.row][move->to.col].type = board[move->from.row][move->from.col].type;
	board[move->to.row][move->to.col].color = board[move->from.row][move->from.col].color;
	board[move->to.row][move->to.col].is_moved = 1;
	board[move->from.row][move->from.col].type = BLANK;
	board[move->from.row][move->from.col].color = EMPTY;
	board[move->from.row][move->from.col].is_moved = 1;
}

uint8_t is_square_safe(Coordinates square, MoveTurn turn) {
	int r, c;

	dlog("checking safety.. (%d,%d)\n", square.row, square.col);

	/* Check Right */
	for (c = square.col + 1; c <= _H_; c++) {
		if ((board[square.row][c].color != turn) && 
				((board[square.row][c].type == ROOK) || 
				(board[square.row][c].type == QUEEN)))
		{
			dlog("right\n");
			return 0;
		}
		else if (board[square.row][c].type != BLANK) {
			break;
		}
	}

	/* Check Left */
	for (c = square.col - 1; c >= _A_; c--) {
		if ((board[square.row][c].color != turn) && 
				((board[square.row][c].type == ROOK) || 
				(board[square.row][c].type == QUEEN)))
		{
			dlog("left\n");
			return 0;
		}
		else if (board[square.row][c].type != BLANK) {
			break;
		}
	}

	/* Check Forward */
	for (r = square.row + 1; r <= _8_; r++) {
		if ((board[r][square.col].color != turn) && 
				((board[r][square.col].type == ROOK) || 
				(board[r][square.col].type == QUEEN)))
		{
			dlog("forward\n");
			return 0;
		}
		else if (board[r][square.col].type != BLANK) {
			break;
		}
	}

	/* Check Backward */
	for (r = square.row - 1; r >= _1_; r--) {
		if ((board[r][square.col].color != turn) && 
				((board[r][square.col].type == ROOK) || 
				(board[r][square.col].type == QUEEN)))
		{
			dlog("backward\n");
			return 0;
		}
		else if (board[r][square.col].type != BLANK) {
			break;
		}
	}

	/* Check Forward-Right */
	r = square.row;
	c = square.col;
	while ((r <= _8_) && (c <= _H_)) {
		if ((board[r][c].color != turn) && 
				((board[r][c].type == BISHOP) || 
				(board[r][c].type == QUEEN)))
		{
			dlog("forward-right\n");
			return 0;
		}
		else if (board[r][c].type != BLANK) {
			break;
		}

		r++;
		c++;
	}

	/* Check Forward-Left */
	r = square.row + 1;
	c = square.col - 1;
	while ((r <= _8_) && (c >= _A_)) {
		if ((board[r][c].color != turn) && 
				((board[r][c].type == BISHOP) || 
				(board[r][c].type == QUEEN)))
		{
			dlog("forward-left\n");
			return 0;
		}
		else if (board[r][c].type != BLANK) {
			break;
		}

		r++;
		c--;
	}
	
	/* Check Backward-Right */
	r = square.row;
	c = square.col;
	while ((r >= _1_) && (c <= _H_)) {
		if ((board[r][c].color != turn) && 
				((board[r][c].type == BISHOP) || 
				(board[r][c].type == QUEEN))) 
		{
			dlog("back-right\n");
			return 0;
		}
		else if (board[r][c].type != BLANK) {
			break;
		}

		r--;
		c++;
	}

	/* Check Backward-Left */
	r = square.row;
	c = square.col;
	while ((r >= _1_) && (c >= _A_)) {
		if ((board[r][c].color != turn) && 
				((board[r][c].type == BISHOP) || 
				(board[r][c].type == QUEEN)))
		{
			dlog("back-left\n");
			return 0;
		}
		else if (board[r][c].type != BLANK) {
			break;
		}

		r--;
		c--;
	}

	/* TODO: Check Knight threat */
	int kv[8][2] = {{1,2}, {1,-2}, {-1,2}, {-1,-2},
				{2,1}, {2,-1}, {-2,1}, {-2,-1}};

	for (int i = 0; i < 8; i++) {
		if ((square.row + kv[i][0] >= _1_) && (square.row + kv[i][0] <= _8_) &&
			(square.col + kv[i][1] >= _A_) && (square.col + kv[i][1] <= _H_) &&
			(board[square.row + kv[i][0]][square.col + kv[i][1]].color != turn) &&
			(board[square.row + kv[i][0]][square.col + kv[i][1]].type == KNIGHT)) 
		{
			dlog("knight\n");
			return 0;
		}
	}
	
	/* Check Pawn threat */
	if (turn == WHITE) {
		if (((square.col + 1) <= _H_) &&
			(board[square.row + 1][square.col + 1].color == BLACK) &&
			(board[square.row + 1][square.col + 1].type == PAWN))
		{
			dlog("pawn 1\n");
			return 0;
		}
		else if (((square.col - 1) >= _A_) &&
			(board[square.row + 1][square.col - 1].color == BLACK) &&
			(board[square.row + 1][square.col - 1].type == PAWN))
		{
			dlog("pawn 2\n");
			return 0;
		}
	}
	else {
		if (((square.col + 1) <= _H_) &&
			(board[square.row - 1][square.col + 1].color == WHITE) &&
			(board[square.row - 1][square.col + 1].type == PAWN))
		{
			dlog("pawn 3\n");
			return 0;
		}
		else if (((square.col - 1) >= _A_) &&
			(board[square.row - 1][square.col - 1].color == WHITE) &&
			(board[square.row - 1][square.col - 1].type == PAWN))
		{
			dlog("pawn 4\n");
			return 0;
		}
	}

	dlog("safety check end\n");
	return 1;
}
