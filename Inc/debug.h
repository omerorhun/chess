#ifndef __DEBUG_H_
#define __DEBUG_H_

#include <wchar.h>

//#define clog(format, args...) printf(format, ##args)
#define clog(format, args...) wprintf(L""format, ##args)

#endif /* __DEBUG_H_ */