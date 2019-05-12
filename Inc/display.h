#ifndef __DISPLAY_H_
#define __DISPLAY_H_

#include <stdint.h>
#include "board.h"
#include "debug.h"

typedef enum {
	LIGHT = 1,
	DARK = 2
}BoardColor;

extern int show_board(void);
extern int show_board_coordinates(void);
extern void set_colors(PieceColor fg, BoardColor bg);
extern void reset_colors();

#endif /* __DISPLAY_H_ */
