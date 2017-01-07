#include <io.h> //_open
#include <fcntl.h> //_O_APPEND
#include <sys\stat.h> //_S_IREAD
#include "MsgBoxError.h" //includes windows.h, errno.h

int WINAPI WinMain(HINSTANCE hInst,
                   HINSTANCE hPrevInst,
                   PSTR szArgs,
                   int iCmdShow)
{
    if (CreateFile(NULL, 0, 0, 0, 0, 0, NULL) == INVALID_HANDLE_VALUE)
        MSGBOX_LASTERR(NULL, L"CreateFile");

    if (!DeleteFile(L"non_existant_file"))
        MSGBOX_LASTERR(NULL, L"DeleteFile");

    if (_open("non_existant_file", _O_APPEND, _S_IREAD) == -1)
        MSGBOX_ERRNO(NULL, L"_open");

    return 0;
}
