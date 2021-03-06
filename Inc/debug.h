#ifndef __DEBUG_H_
#define __DEBUG_H_

#include <wchar.h>
#include "conf.h"

#if NCURSES_ENABLED
#include <ncurses.h>
#endif

//#define dlog(format, args...) printf(format, ##args)


#if NCURSES_ENABLED
#define log(format, args...) printw(format, ##args)
#define disp(format, args...) printw(format, ##args)

#if DEBUG_ENABLED
#define dlog(format, args...) {if (line_log >= 35) {\
col_log += 50;\
col_log %= 100;\
 if (col_log == 1) {\
 	wclear(winlogs);\
	box(winlogs, 0, 0);\
}\
line_log = 1;}\
mvwprintw(winlogs, line_log++, col_log, format, ##args),\
refresh(), wrefresh(winlogs);}
#else
#define dlog(format, args...) 
#endif

WINDOW *winlogs;
extern void init_log(void);
int line_log;
int col_log;

#else
#define log(format, args...) wprintf(L""format, ##args)
#define disp(format, args...) wprintf(L""format, ##args)

#if DEBUG_ENABLED
#define dlog(format, args...) wprintf(L""format, ##args)
#else
#define dlog(format, args...) 
#endif
#endif

#endif /* __DEBUG_H_ */