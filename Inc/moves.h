#ifndef __MOVES_H_
#define __MOVES_H_

#include "board.h"
#include "error.h"

extern Coordinates find_king(Coordinates target, PieceColor color);
extern Coordinates find_knight(Coordinates target, PieceColor color);

extern Coordinates find_starting_point(Coordinates target, Piece attacker);
extern Coordinates find_straight(Coordinates target, PieceColor color, Piece types);
extern Coordinates find_cross(Coordinates target, PieceColor color, Piece types);

#endif /* __MOVES_H_ */
