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

ErrorCodes make_move(MoveCoordinates move) {
	ErrorCodes ret = ERR_OK;
	
	/* Check the move */
	ret = check_move_if_valid(move);
	
	if (ret == ERR_OK) {

		/* Check extra move */
		if (g_extra_move == SHORT_ROOK) {
			MoveCoordinates move_rook = {move.from_r, _H_, move.to_r, _F_};
			move_piece(&move_rook);
		}
		else if(g_extra_move == LONG_ROOK) {
			MoveCoordinates move_rook = {move.from_r, _A_, move.to_r, _D_};
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
			dlog("en passant available\n");
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
	PieceInfo piece = get_piece_info(move.from_r, move.from_c);

	if (piece.is_moved) {
		printf("piece is moved\n");
		return ERR_INVALID_MOVE_ROOK;
	}
	
	if (!is_square_safe(piece.location, g_turn))
		return ERR_INVALID_MOVE_ROOK_KING_UNDERATTACK;
	
	if (get_col_dist(move) == 0) {
		if ((get_row_dist(move) == 2) && !(board[move.from_r][_H_].is_moved)) {
			/* TODO: Yol üzerinde taş var mı kontrol et */
			MoveCoordinates way = {move.from_r, move.from_c, move.to_r, _H_};
			
			// dlog("checking route %d\n", __LINE__);
			// dlog("%d-%d to %d-%d\n", way.from_r, way.from_c, way.to_r, way.to_c);

			if (check_the_route(&way) != ERR_OK)
				return ERR_INVALID_MOVE_NOT_CLEAR_ROUTE;

			/* TODO: Yolu gören rakip taş var mı kontrol et */
			for (int i = move.from_c + 1; i <= move.to_c; i++) {
				Coordinates coor = {move.from_r, i};
				if (!is_square_safe(coor, g_turn))
					return ERR_INVALID_MOVE_ROOK_KINGS_ROUTE_NOT_SAFE;
			}

			g_extra_move = SHORT_ROOK;

			return ERR_OK;
		}
		else if(get_row_dist(move) == -2 && !(board[move.from_r][_A_].is_moved)) {
			/* TODO: Yol üzerinde taş var mı kontrol et */
			MoveCoordinates way = {move.from_r, move.from_c, move.to_r, _A_};
			
			// dlog("checking route %d\n", __LINE__);
			// dlog("%d-%d to %d-%d\n", way.from_r, way.from_c, way.to_r, way.to_c);
			
			if (check_the_route(&way) != ERR_OK)
				return ERR_INVALID_MOVE_NOT_CLEAR_ROUTE;

			/* TODO: Yolu gören rakip taş var mı kontrol et */
			for (int i = move.from_c - 1; i >= move.to_c; i--) {
				Coordinates coor = {move.from_r, i};
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
	board[move->from_r][move->from_c].is_moved = 1;
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
		else if (board[r][square.col].type != BLANK) {
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
		else if (board[r][square.col].type != BLANK) {
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
		else if (board[r][square.col].type != BLANK) {
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
		else if (board[r][square.col].type != BLANK) {
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
		else if (board[r][square.col].type != BLANK) {
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
		else if (board[r][square.col].type != BLANK) {
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
			(board[square.row - 1][square.col + 1].color == BLACK) &&
			(board[square.row - 1][square.col + 1].type == PAWN))
		{
			dlog("pawn 3\n");
			return 0;
		}
		else if (((square.col - 1) >= _A_) &&
			(board[square.row - 1][square.col - 1].color == BLACK) &&
			(board[square.row - 1][square.col - 1].type == PAWN))
		{
			dlog("pawn 4\n");
			return 0;
		}
	}

	return 1;
}
