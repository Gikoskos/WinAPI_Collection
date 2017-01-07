#include "MsgBoxError.h" //includes windows.h

int WINAPI WinMain(HINSTANCE hInst,
                   HINSTANCE hPrevInst,
                   PSTR szArgs,
                   int iCmdShow)
{
    if (CreateFile(NULL, 0, 0, 0, 0, 0, NULL) == INVALID_HANDLE_VALUE)
        MSGBOX_LASTERR(NULL, L"CreateFile");

    if (!DeleteFile(L"no_file"))
        MSGBOX_LASTERR(NULL, L"DeleteFile");

    return 0;
}
