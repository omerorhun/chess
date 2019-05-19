#ifndef __BOARD_H_
#define __BOARD_H_

#include <stdint.h>
#include "error.h"
#include "debug.h"

#define STARTING_POSITION {\
	{\
	{ROOK,WHITE,{_1_,_A_},0,0}, {KNIGHT,WHITE,{_1_,_B_},0,0}, {BISHOP,WHITE,{_1_,_C_},0,0}, {QUEEN,WHITE,{_1_,_D_},0,0}, \
	{KING,WHITE,{_1_,_E_},0,0}, {BISHOP,WHITE,{_1_,_F_},0,0}, {KNIGHT,WHITE,{_1_,_G_},0,0}, {ROOK,WHITE,{_1_,_H_},0,0}},\
	{\
	{PAWN,WHITE,{_2_,_A_},0,0}, {PAWN,WHITE,{_2_,_B_},0,0}, {PAWN,WHITE,{_2_,_C_},0,0}, {PAWN,WHITE,{_2_,_D_},0,0}, \
	{PAWN,WHITE,{_2_,_E_},0,0}, {PAWN,WHITE,{_2_,_F_},0,0}, {PAWN,WHITE,{_2_,_G_},0,0}, {PAWN,WHITE,{_2_,_H_},0,0}},\
	{\
	{0,0,{_3_,_A_},0,0}, {0,0,{_3_,_B_},0,0}, {0,0,{_3_,_C_},0,0}, {0,0,{_3_,_D_},0,0}, \
	{0,0,{_3_,_E_},0,0}, {0,0,{_3_,_F_},0,0}, {0,0,{_3_,_G_},0,0}, {0,0,{_3_,_H_},0,0}},\
	{\
	{0,0,{_4_,_A_},0,0}, {0,0,{_4_,_B_},0,0}, {0,0,{_4_,_C_},0,0}, {0,0,{_4_,_D_},0,0}, \
	{0,0,{_4_,4},0,0}, {0,0,{_4_,_F_},0,0}, {0,0,{_4_,_G_},0,0}, {0,0,{_4_,_H_},0,0}},\
	{\
	{0,0,{_5_,_A_},0,0}, {0,0,{_5_,_B_},0,0}, {0,0,{_5_,_C_},0,0}, {0,0,{_5_,_D_},0,0}, \
	{0,0,{_5_,_E_},0,0}, {0,0,{_5_,_F_},0,0}, {0,0,{_5_,_G_},0,0}, {0,0,{_5_,_H_},0,0}},\
	{\
	{0,0,{_6_,_A_},0,0}, {0,0,{_6_,_B_},0,0}, {0,0,{_6_,_C_},0,0}, {0,0,{_6_,_D_},0,0}, \
	{0,0,{_6_,_E_},0,0}, {0,0,{_6_,_F_},0,0}, {0,0,{_6_,_G_},0,0}, {0,0,{_6_,_H_},0,0}},\
	{\
	{PAWN,BLACK,{_7_,_A_},0,0}, {PAWN,BLACK,{_7_,_B_},0,0}, {PAWN,BLACK,{_7_,_C_},0,0}, {PAWN,BLACK,{_7_,_D_},0,0}, \
	{PAWN,BLACK,{_7_,_E_},0,0}, {PAWN,BLACK,{_7_,_F_},0,0}, {PAWN,BLACK,{_7_,_G_},0,0}, {PAWN,BLACK,{_7_,_H_},0,0}},\
	{\
	{ROOK,BLACK,{_8_,_A_},0,0}, {KNIGHT,BLACK,{_8_,_B_},0,0}, {BISHOP,BLACK,{_8_,_C_},0,0}, {QUEEN,BLACK,{_8_,_D_},0,0}, \
	{KING,BLACK,{_8_,_E_},0,0}, {BISHOP,BLACK,{_8_,_F_},0,0}, {KNIGHT,BLACK,{_8_,_G_},0,0}, {ROOK,BLACK,{_8_,_H_},0,0}},\
	}

enum {
	_A_,
	_B_,
	_C_,
	_D_,
	_E_,
	_F_,
	_G_,
	_H_,
	COL_COUNT
}EnCols;

enum {
	_1_,
	_2_,
	_3_,
	_4_,
	_5_,
	_6_,
	_7_,
	_8_,
	ROW_COUNT
}EnRows;

typedef struct {
	uint8_t row;
	uint8_t col;
}Coordinates;

typedef enum {
	BLANK,
	PAWN = (1 << 1), //0x265F,
	KNIGHT = (1 << 2), //0x265E,
	BISHOP = (1 << 3), //0x265D,
	ROOK = (1 << 4), //0x265C,
	QUEEN = (1 << 5), //0x265B,
	KING = (1 << 6), //0x265A
}Piece;

typedef enum {
	EMPTY,
	WHITE = 1,
	BLACK = 2
}PieceColor;

typedef struct {
	Piece type;
	uint8_t color;
	const Coordinates location;
	uint8_t value;
	uint8_t is_locked;
	uint8_t is_moved;
}PieceInfo;

typedef struct {
	/* TODO: Coordinates tipinde tanımlanabilir */
	Coordinates from;
	Coordinates to;
	// uint8_t from_r;
	// uint8_t from_c;
	// uint8_t to_r;
	// uint8_t to_c;
}MoveCoordinates;

typedef enum {
	WHITE_TURN = 0, BLACK_TURN = !WHITE_TURN
}MoveTurn;

typedef enum {
	NO_EXTRA_MOVE,
	LONG_ROOK,
	SHORT_ROOK,
	ENPASSANT
}ExtraMove;

extern PieceInfo board[ROW_COUNT][COL_COUNT];

ErrorCodes make_move(MoveCoordinates move);

#endif /* __BOARD_H_ */
