//---------------------------------------------------------------------------


#pragma hdrstop

#include "Utils.h"
#include "Log.h"
#include <dir.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)

void DeleteDirectory(HWND WindowHandle, AnsiString FromDir, bool Silent) 
{
	SHFILEOPSTRUCT ShFileOpStruct;
	memset(&ShFileOpStruct, 0, sizeof(SHFILEOPSTRUCT));
	char folder[1024];
	memset(folder, 0, sizeof(folder));
	if (FromDir.Length() >= (int)sizeof(folder))
		return;
	strcpy(folder, FromDir.c_str());

	ShFileOpStruct.hwnd                   = WindowHandle;
	ShFileOpStruct.wFunc                  = FO_DELETE;
	ShFileOpStruct.pFrom                  = folder;
	ShFileOpStruct.pTo                    = 0;
	if (Silent)
		ShFileOpStruct.fFlags     = FOF_NOCONFIRMATION + FOF_SILENT;
	else
		ShFileOpStruct.fFlags     = FOF_NOCONFIRMATION;
	ShFileOpStruct.fAnyOperationsAborted  = false;
	ShFileOpStruct.hNameMappings          = NULL;
	ShFileOpStruct.lpszProgressTitle      = NULL;

	int ret = SHFileOperation(&ShFileOpStruct);
	if (ret)
	{
		LOG("Failed to delete folder %s", FromDir.c_str());
	}
}

// Append the newDir directory to the path, retaining any
// filename that might already be on the path.
//
AnsiString appendDirectory(AnsiString path, AnsiString newDir)
{
	char drillIntoPath[MAXPATH];
	char drive[MAXDRIVE];
	char dir[MAXDIR];
	char file[MAXFILE];
	char ext[MAXEXT];

	fnsplit(path.c_str(),drive,dir,file,ext);

	strcat(dir, newDir.c_str());

	fnmerge(drillIntoPath,drive,dir,file,ext);

	return(AnsiString(drillIntoPath));
}
