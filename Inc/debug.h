#ifndef __DEBUG_H_
#define __DEBUG_H_

#include <wchar.h>

//#define dlog(format, args...) printf(format, ##args)
#define dlog(format, args...) wprintf(L""format, ##args)
//#define dlog(format, args...) 
#define log(format, args...) wprintf(L""format, ##args)
#define disp(format, args...) wprintf(L""format, ##args)

#endif /* __DEBUG_H_ */