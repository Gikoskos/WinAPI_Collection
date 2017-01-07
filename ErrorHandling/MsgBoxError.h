#ifndef __MSGBOXERR_HDR_H__
#define __MSGBOXERR_HDR_H__

#include "..\Common.h"

#ifdef _MSC_VER
# define FUNCSTR __FUNCTIONW__
#else
# define FUNCSTR (PCWSTR)ToWideStr(__func__)
#endif


//Error handling

//flags for the __MsgBoxDefaultFuncError function
#define __GETLASTERR 0x00
#define __GETERRNO 0x01

#define MSGBOX_ERR(x, y, z) \
    do { \
        MsgBoxDefaultFuncError(x, y, FUNCSTR, __LINE__ - 1, z, __GETLASTERR); \
    } while (0) \

#define MSGBOX_LASTERR(x, y) \
    do { \
        int err = GetLastError(); \
        MsgBoxDefaultFuncError(x, y, FUNCSTR, __LINE__ - 1, err, __GETLASTERR); \
    } while (0) \

#define MSGBOX_ERRNO(x, y) \
    do { \
        int err = errno; \
        MsgBoxDefaultFuncError(x, y, FUNCSTR, __LINE__ - 1, err, __GETERRNO); \
    } while (0) \

#define MSGBOX_WSAERR(x, y) \
    do { \
        int err = WSAGetLastError(); \
        MsgBoxDefaultFuncError(x, y, FUNCSTR, __LINE__ - 1, err, __GETLASTERR); \
    } while (0) \

//MsgBoxError.c
PWSTR ToWideStr(PCSTR to_convert);
void MsgBoxDefaultFuncError(HWND hwnd, PCWSTR failed_func, PCWSTR caller_func, const ULONG line, const INT err,  const int flag);


#endif //__MSGBOXERR_HDR_H__