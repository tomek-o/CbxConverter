//---------------------------------------------------------------------------

#ifndef UtilsH
#define UtilsH
//---------------------------------------------------------------------------

#include <System.hpp>

void DeleteDirectory(HWND WindowHandle, AnsiString FromDir, bool Silent);

AnsiString appendDirectory(AnsiString path, AnsiString newDir);

#endif
