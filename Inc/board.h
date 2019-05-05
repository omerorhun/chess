#ifndef __BOARD_H_
#define __BOARD_H_

#include <stdint.h>
#include "error.h"

#define STARTING_POSITION {\
	{\
	{0,0,{0,0},0,0}, {0,0,{0,1},0,0}, {0,0,{0,2},0,0}, {0,0,{0,3},0,0}, \
	{0,0,{0,4},0,0}, {0,0,{0,5},0,0}, {0,0,{0,6},0,0}, {0,0,{0,7},0,0}},\
	{\
	{0,0,{1,0},0,0}, {0,0,{1,1},0,0}, {0,0,{1,2},0,0}, {0,0,{1,3},0,0}, \
	{0,0,{1,4},0,0}, {0,0,{1,5},0,0}, {0,0,{1,6},0,0}, {0,0,{1,7},0,0}},\
	{\
	{0,0,{2,0},0,0}, {0,0,{2,1},0,0}, {0,0,{2,2},0,0}, {0,0,{2,3},0,0}, \
	{0,0,{2,4},0,0}, {0,0,{2,5},0,0}, {0,0,{2,6},0,0}, {0,0,{2,7},0,0}},\
	{\
	{0,0,{3,0},0,0}, {0,0,{3,1},0,0}, {0,0,{3,2},0,0}, {0,0,{3,3},0,0}, \
	{0,0,{3,4},0,0}, {0,0,{3,5},0,0}, {0,0,{3,6},0,0}, {0,0,{3,7},0,0}},\
	{\
	{0,0,{4,0},0,0}, {0,0,{4,1},0,0}, {0,0,{4,2},0,0}, {0,0,{4,3},0,0}, \
	{0,0,{4,4},0,0}, {0,0,{4,5},0,0}, {0,0,{4,6},0,0}, {0,0,{4,7},0,0}},\
	{\
	{0,0,{5,0},0,0}, {0,0,{5,1},0,0}, {0,0,{5,2},0,0}, {0,0,{5,3},0,0}, \
	{0,0,{5,4},0,0}, {0,0,{5,5},0,0}, {0,0,{5,6},0,0}, {0,0,{5,7},0,0}},\
	{\
	{0,0,{6,0},0,0}, {0,0,{6,1},0,0}, {0,0,{6,2},0,0}, {0,0,{6,3},0,0}, \
	{0,0,{6,4},0,0}, {0,0,{6,5},0,0}, {0,0,{6,6},0,0}, {0,0,{6,7},0,0}},\
	{\
	{0,0,{7,0},0,0}, {0,0,{7,1},0,0}, {0,0,{7,2},0,0}, {0,0,{7,3},0,0}, \
	{0,0,{7,4},0,0}, {0,0,{7,5},0,0}, {0,0,{7,6},0,0}, {0,0,{7,7},0,0}}\
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
	PAWN,
	KNIGHT,
	BISHOP,
	ROOK,
	QUEEN,
	KING
}Piece;

typedef enum {
	EMPTY,
	WHITE,
	BLACK
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
	uint8_t from_c;
	uint8_t from_r;
	uint8_t to_c;
	uint8_t to_r;
}MoveCoordinates;

/* TODO: Burada kaldi const struct elemanlarina ilk degerleri atanacak */
extern PieceInfo board[ROW_COUNT][COL_COUNT];

void init_board(void);
ErrorCodes make_move(MoveCoordinates move);

#endif /* __BOARD_H_ */
