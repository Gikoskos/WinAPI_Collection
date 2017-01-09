#include <Common.h>

#include <tlhelp32.h>
#include <stdio.h>
#include <wchar.h>

#define P_ERR(func) \
    if (GetLastError()) { \
        fprintf(stderr, "%s failed with GetLastError == %ld", func, GetLastError()); \
    }

/**
 * @brief Fill a PROCESSENTRY32W array with data from all running processes
 *
 * Get all PROCESSENTRY32W data for every process that's currently running
 * on your Windows system.
 *
 * @param pe32List
 * @return The number of running processes on the system, or 0 on error.
 */
DWORD GetRunningProcesses(_Out_ PROCESSENTRY32W **pe32List)
{
    static const DWORD dwDefaultAllocSize = 50;

    HANDLE hProcessSnap = NULL;
    PROCESSENTRY32W pe32CurrProc = {.dwSize = sizeof(PROCESSENTRY32W)};
    DWORD dwAlloc = dwDefaultAllocSize, dwProcNum = 0;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        P_ERR("CreateToolhelp32Snapshot");
        goto CLEANUP;
    }

    if (!Process32FirstW(hProcessSnap, &pe32CurrProc)) {
        P_ERR("Process32First");
        goto CLEANUP;
    }

    *pe32List = win_malloc(sizeof(PROCESSENTRY32W) * dwAlloc);

    do {
       if (dwProcNum >= dwAlloc) {
           dwAlloc += dwDefaultAllocSize;
           *pe32List = win_realloc(*pe32List, sizeof(PROCESSENTRY32W) * dwAlloc);
       }

       CopyMemory(&(*pe32List)[dwProcNum++], &pe32CurrProc, sizeof(PROCESSENTRY32W));

    } while (Process32NextW(hProcessSnap, &pe32CurrProc));

CLEANUP:
    if (hProcessSnap) CloseHandle(hProcessSnap);

    return dwProcNum;
}

int wmain(int argc, wchar_t **argv)
{
    PROCESSENTRY32W *pe32List = NULL;
    DWORD dwRunningProc;

    dwRunningProc = GetRunningProcesses(&pe32List);
    if (dwRunningProc)
        for (DWORD i = 0; i < dwRunningProc; i++)
            wprintf(L"PID: %04ld\tPNAME: %s\r\n", pe32List[i].th32ProcessID, pe32List[i].szExeFile);

    if (pe32List) win_free(pe32List);
    return 0;
}
