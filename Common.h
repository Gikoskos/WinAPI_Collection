#ifndef __COMMON_HDR_H__
#define __COMMON_HDR_H__

#ifdef _MSC_VER
# pragma comment(lib, "user32.lib")
# pragma comment(lib, "kernel32.lib")
#endif


#ifndef UNICODE
# define UNICODE 1
#endif

#ifndef _UNICODE
# define _UNICODE 1
#endif


#include <windows.h>
#include <errno.h>
#include <tchar.h>

#ifndef _MSC_VER
# undef __CRT__NO_INLINE
#endif
#include <strsafe.h> //win32 native string handling

#ifndef _WINAPI_EASY_HEAP_MACROS
# define win_free(x) HeapFree(GetProcessHeap(), 0, x)
# define win_malloc(x) HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, x)
# define win_realloc(x, y) HeapReAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, x, y)
# define win_calloc(x, y) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY | HEAP_GENERATE_EXCEPTIONS, (x) * y)
# define _WINAPI_EASY_HEAP_MACROS 1
#endif


#endif //__COMMON_HDR_H__
