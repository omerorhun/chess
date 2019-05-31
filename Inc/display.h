#ifndef __DISPLAY_H_
#define __DISPLAY_H_

#include <stdint.h>
#include "board.h"
#include "debug.h"
#include "conf.h"

#if NCURSES_ENABLED
#include <ncurses.h>
#include <locale.h>
#include <wchar.h>
#endif

typedef enum {
	LIGHT = 1,
	DARK = 2
}BoardColor;

extern int show_board(void);
extern int show_board_coordinates(void);
extern void set_colors(PieceColor fg, BoardColor bg);
extern void reset_colors();
wchar_t get_piece_unicode(Piece piece);

#if NCURSES_ENABLED

#define BOARD_BOX_HEIGHT 11
#define BOARD_BOX_WIDTH 28
#define BOARD_BOX_Y 1
#define BOARD_BOX_X 2

enum {
	COLOR_DARK = 1,
	COLOR_LIGHT,
	DARK_WHITE,
	DARK_BLACK,
	LIGHT_WHITE,
	LIGHT_BLACK,
	WHITE_ON_DEFAULT,
	SELECT_DARK_WHITE,
	SELECT_DARK_BLACK,
	SELECT_LIGHT_WHITE,
	SELECT_LIGHT_BLACK
};

WINDOW *winboard;

extern ErrorCodes init_display(void);
extern void nc_draw_board(WINDOW **window, int h, int w, int y, int x);
extern void nc_display_move(MoveCoordinates mc);
extern void focus_square(int x, int y);

WINDOW *winboard_test;

void show_current_board(void);

#endif

#endif /* __DISPLAY_H_ */
