#ifndef __MOVES_H_
#define __MOVES_H_

#include "board.h"
#include "error.h"


typedef struct move_list{
    int no;
    MoveCoordinates mc;
    struct move_list *prev;
    struct move_list *next;
}MoveList;

extern MoveList *moves;
extern MoveList *current_move;
extern int move_count;

extern MoveList *add(MoveList **root, MoveCoordinates new_mc_data);
extern MoveList *add_to(MoveList **root, int index, MoveCoordinates new_mc_data);
extern MoveList *get(MoveList *root, int index);
extern MoveList *get_next(MoveList *node);
extern MoveList *get_prev(MoveList *node);

extern MoveList *get_head(MoveList *node);
extern MoveList *get_tail(MoveList *node);
extern void remove_from(MoveList *node);
extern void remove_move(MoveList *node);

void show_list(MoveList *root);

extern Coordinates find_king(Coordinates target, PieceColor color);
extern Coordinates find_knight(Coordinates target, PieceColor color);

extern Coordinates find_starting_point(Coordinates target, Piece attacker);
extern Coordinates find_straight(Coordinates target, PieceColor color, Piece types);
extern Coordinates find_cross(Coordinates target, PieceColor color, Piece types);

#endif /* __MOVES_H_ */
