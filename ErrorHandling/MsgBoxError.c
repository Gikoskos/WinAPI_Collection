#include "MsgBoxError.h"

PWSTR ToWideStr(PCSTR to_convert)
{
    wchar_t *wide_str;
    int len = lstrlenA(to_convert) + 1;

    wide_str = win_malloc(sizeof(wchar_t) * len);

    MultiByteToWideChar(CP_UTF8, 0, to_convert, len, wide_str, len);

    return wide_str;
}

void MsgBoxDefaultFuncError(HWND hwnd,
                            PCWSTR szFailedFuncName,
                            PCWSTR szCallerFuncName,
                            const ULONG uLine,
                            const INT err,
                            const int flag)
{ 
    PVOID pDisplayBuf;

    if (flag == __GETLASTERR && err) {
        PVOID formatted_buff;

        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                       FORMAT_MESSAGE_FROM_SYSTEM |
                       FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, err, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                       (PWSTR)&formatted_buff, 0, NULL);

        pDisplayBuf = win_calloc(lstrlenW(formatted_buff) + 
                                 100 +
                                 lstrlenW(szFailedFuncName) +
                                 lstrlenW(szCallerFuncName), sizeof(wchar_t));

        StringCchPrintfW((PWSTR)pDisplayBuf, LocalSize(pDisplayBuf) / sizeof(wchar_t),
                         L"%s() call failed in function %s() line %lu\r\nGetLastError %lu: %s",
                         szFailedFuncName, szCallerFuncName, uLine, err, formatted_buff);

        MessageBoxExW(hwnd, (PCWSTR)pDisplayBuf, L"Something happened!", MB_OK | MB_ICONERROR,
                      MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));

        //fwprintf(stderr, L"== Error at %s():%s():%lu\r\n-> %s ==\n", szCallerFuncName, szFailedFuncName, uLine, formatted_buff);

#ifndef _MSC_VER
        win_free((PWSTR)szCallerFuncName);
#endif
        win_free(pDisplayBuf);
        LocalFree(formatted_buff);
    } else if (flag == __GETERRNO) {
        char tmperrno_buff[255];
        wchar_t *errno_buff;

        strerror_s(tmperrno_buff, 255, err);

        errno_buff = ToWideStr(tmperrno_buff);

        pDisplayBuf = win_calloc(lstrlenW(errno_buff) +
                                 100 +
                                 lstrlenW(szFailedFuncName) +
                                 lstrlenW(szCallerFuncName), sizeof(wchar_t));

        StringCchPrintfW((PWSTR)pDisplayBuf, LocalSize(pDisplayBuf) / sizeof(wchar_t),
                         L"%s() call failed in function %s() line %lu\r\nerrno %lu: %s",
                         szFailedFuncName, szCallerFuncName, uLine, err, errno_buff);

        MessageBoxExW(hwnd, (PCWSTR)pDisplayBuf, L"Something happened!", MB_OK | MB_ICONERROR,
                      MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));

        //fwprintf(stderr, L"== Error at %s():%s():%lu -> %s ==\n", caller_func, szFailedFuncName, uLine, tmperrno_buff);

#ifndef _MSC_VER
        win_free((PWSTR)szCallerFuncName);
#endif
        win_free(errno_buff);
        win_free(pDisplayBuf);
    }
}
