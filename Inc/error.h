#ifndef __ERROR_H_
#define __ERROR_H_

typedef enum {
	ERR_OK,
	ERR_WRONG_INPUT_FORMAT = 1,
	ERR_INVALID_MOVE = 2,
	ERR_BLANK_MOVE = 3,
	ERR_TEST = 4,
	ERR_INVALID_MOVE_LOCKED_PIECE = 5,
	ERR_INVALID_MOVE_WRONG_COORDINATES = 6,
	ERR_INVALID_MOVE_PAWN_MUST_GO_FORWARD = 7,
	ERR_INVALID_MOVE_BISHOP_NOT_DIAGONAL = 8,
	ERR_INVALID_MOVE_KNIGHT_NOT_L = 9,
	ERR_INVALID_MOVE_ROOK_NOT_STRAIGHT = 10,
	ERR_INVALID_MOVE_QUEEN_NOT_STRAIGHT_OR_DIAGONAL = 11,
	ERR_INVALID_MOVE_KING_WRONG_WAY = 12,
	ERR_INVALID_MOVE_ROOK = 13,
	ERR_INVALID_MOVE_NOT_CLEAR_ROUTE = 14,
	ERR_INVALID_MOVE_FRIENDLY_FIRE = 15,
	ERR_OK_ROOK_LONG = 16,
	ERR_OK_ROOK_SHORT = 17,
	ERR_WRONG_TURN = 18,
	ERR_NOT_ENPASSANT_MOVE = 19,
	ERR_INVALID_MOVE_ROOK_KING_UNDERATTACK = 20,
	ERR_INVALID_MOVE_ROOK_KINGS_ROUTE_NOT_SAFE = 21,
	ERR_WRONG_NOTATION = 22,
	ERR_NO_COLOR_SUPPORT = 23
}ErrorCodes;

#endif /* __ERROR_H_ */
