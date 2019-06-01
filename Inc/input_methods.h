#ifndef __INPUT_METHODS_H_
#define __INPUT_METHODS_H_

#include "error.h"
#include "board.h"

#if NCURSES_ENABLED
WINDOW *wininput;
int line_in;

typedef enum {
    INPUT_READY,
    INPUT_WAITING_FIRST_COORDINATE,
    INPUT_WAITING_SECOND_COORDINATE
}InputStates;

extern void init_input(void);
extern InputStates get_mouse_input(MoveCoordinates *mc);
extern void init_mouse(void);
extern void refresh_move(MoveCoordinates *mc);
#endif

extern ErrorCodes parse_human_input(char *human_input, MoveCoordinates *move);
extern ErrorCodes parse_notation(char *notation, MoveCoordinates *move);

#endif /* __INPUT_METHODS_H_ */
