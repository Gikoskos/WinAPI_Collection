/* API to write text to an open instance of Windows Notepad */

#ifndef UNICODE
# define UNICODE
#endif

#ifndef _UNICODE
# define _UNICODE
#endif

#include <windows.h>
#include <windowsx.h> //Edit_SetSel, Edit_ReplaceSel
#include <stdio.h> //fprintf
#include <tlhelp32.h>

#define P_ERR(func) \
    if (GetLastError()) { \
        fprintf(stderr, "%s failed with GetLastError == %d", func, GetLastError()); \
    }

typedef struct __NOTEPADWINDOW {
    HANDLE hWnd;
    DWORD id;
} NOTEPADWINDOW;



void EditControlAppend(_In_ HWND hEditCtl, _In_ PCWSTR szToAppend)
{
    // move the caret to the end of the text
    int outLength = GetWindowTextLengthW(hEditCtl);
    Edit_SetSel(hEditCtl, outLength, outLength);

    // insert the text at the new caret position
    Edit_ReplaceSel(hEditCtl, szToAppend);
}

BOOL CALLBACK EnumWindowsProc(_In_ HWND hWnd, _In_ LPARAM lParam)
{
    NOTEPADWINDOW *notepad = (NOTEPADWINDOW*)lParam;
    DWORD pid;

    GetWindowThreadProcessId(hWnd, &pid);

    //printf("id in EnumWindowsProc: %ld\n", notepad->id);
    if (pid == notepad->id && !GetWindow(hWnd, GW_OWNER) && IsWindowVisible(hWnd)) {
        notepad->hWnd = hWnd;
        return FALSE;
    }

    return TRUE;
}

BOOL WriteToOpenNotepadW(_In_ PCWSTR szText, _In_ BOOL fAppendText)
{
    HANDLE hProcessSnap = NULL, hNotepadEditCtl;
    PROCESSENTRY32W pe32;
    NOTEPADWINDOW notepadProc = {.hWnd = NULL};
    BOOL ret = FALSE;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        P_ERR("CreateToolhelp32Snapshot");
        goto CLEANUP;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if ( !Process32FirstW(hProcessSnap, &pe32) ) {
        P_ERR("Process32First");
        goto CLEANUP;
    }

    do {
        if ( !lstrcmpW(L"notepad.exe", pe32.szExeFile) ) {

            notepadProc.id = pe32.th32ProcessID;

            //printf("id in WriteToOpenNotepad: %ld\n", notepadProc.id);
            EnumWindows(EnumWindowsProc, (LPARAM)&notepadProc);

            if (notepadProc.hWnd) {
                hNotepadEditCtl = FindWindowExW(notepadProc.hWnd, NULL, L"Edit", NULL);
                if (!hNotepadEditCtl) {
                    P_ERR("FindWindowExW");
                    break;
                }

                if (fAppendText)
                    EditControlAppend(hNotepadEditCtl, szText);
                else
                    SendMessageW(hNotepadEditCtl, WM_SETTEXT, (WPARAM)0, (LPARAM)szText);

                ret = TRUE;
            }

            //comment this line if you want the text to appear on all open instances of notepad.exe
            goto CLEANUP;
        }

    } while( Process32NextW(hProcessSnap, &pe32) );

CLEANUP:
    if (hProcessSnap) CloseHandle(hProcessSnap);

    return ret;
}

BOOL WriteToOpenNotepadAnimatedW(_In_ PCWSTR szText,
                                 _In_ int iTextLen,
                                 _In_ DWORD dwMillis)
{
    wchar_t t[2] = {[1] = L'\0'};
    int i = iTextLen - 1;

    for (; i > -1; i--) {

        t[0] = szText[i];
        if (!WriteToOpenNotepadW(t, TRUE))
            return FALSE;

        Sleep(dwMillis);
    }

    return TRUE;
}

int wmain(int argc, wchar_t **argv)
{
    wchar_t text[] = L"U HAV BEEN HAXXED by \r\n L33t HAxx0R GIKOSKOS \r\n GIB MONI PLZ";
    int arr_sz = sizeof text / sizeof *text;

    WriteToOpenNotepadAnimatedW(text, arr_sz, 10);

    return 0;
}
