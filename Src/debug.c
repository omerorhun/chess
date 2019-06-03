#include "debug.h"
#include "display.h"

#if NCURSES_ENABLED
void init_log(void) {
    line_log = 1;
    col_log = 1;
    winlogs = newwin(40, 0, 1, 40);
	
    wattron(winlogs, COLOR_PAIR(WHITE_ON_DEFAULT));
    box(winlogs, 0, 0);
    
	refresh();
	wrefresh(winlogs);
}
#endif