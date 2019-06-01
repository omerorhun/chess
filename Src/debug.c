#include "debug.h"
#include "display.h"

#if NCURSES_ENABLED
void init_log(void) {
    line_log = 1;
    winlogs = newwin(30, 50, 1, 80);
	
    wattron(winlogs, COLOR_PAIR(WHITE_ON_DEFAULT));
    box(winlogs, 0, 0);
    
	refresh();
	wrefresh(winlogs);
}
#endif