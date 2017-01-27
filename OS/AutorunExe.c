#include <windows.h>
#include <stdio.h>


BOOL EnableModuleAutorun(PCWSTR szFileName)
{
    HKEY hAutorunRegKey = NULL;
    wchar_t szExePath[1024];

    if (!GetModuleFileNameW(NULL, szExePath, 1024) || GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        return FALSE;

    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                      0,
                      KEY_ALL_ACCESS,
                      &hAutorunRegKey) != ERROR_SUCCESS)
        return FALSE;

    if (RegSetValueExW(hAutorunRegKey,
                       szFileName,
                       0,
                       REG_SZ,
                       (LPBYTE)szExePath,
                       sizeof(wchar_t)*(lstrlenW(szExePath) + 1)) != ERROR_SUCCESS) {
        RegCloseKey(hAutorunRegKey);
        return FALSE;
    }

    RegCloseKey(hAutorunRegKey);

    return TRUE;
}

int wmain(int argc, wchar_t **argv)
{
    EnableModuleAutorun(argv[0]);
    MessageBoxW(NULL, L"AutorunExe.exe is running!", L"Attention!", MB_OK | MB_ICONEXCLAMATION);

    return 0;
}
