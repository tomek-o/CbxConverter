//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("FormMain.cpp", frmMain);
USEFORM("FormSettings.cpp", frmSettings);
USEFORM("FormAbout.cpp", frmAbout);
USEFORM("LogUnit.cpp", frmLog);
USEFORM("FormResizeCfg.cpp", frmResizeCfg);
USEFORM("FormRename.cpp", frmRename);
//---------------------------------------------------------------------------
#include "WindowsMessageFilter.h"

WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
#if 0
		// identify test
		int width = 0, height = 0;
		AnsiString name = "D:\\progs\\Gry\\ROTMG\\Clipboard1.png";
		if (Image::Identify(name, width, height) == 0)
		{
			AnsiString params;
			params.sprintf("width = %d, height = %d", width, height);
			ShowMessage(params);
		}
		else
		{
			ShowMessage("Not identified!");
		}
#endif

		/* 	Allows passing WM_DROPFILES to elevated app in Vista+.
			This would be most likely needed only for debugging (TC++ 2006 requires running with admin privileges).
		*/
		ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);

		Application->Title = "CbxConverter";
		Application->CreateForm(__classid(TfrmMain), &frmMain);
		Application->CreateForm(__classid(TfrmSettings), &frmSettings);
		Application->CreateForm(__classid(TfrmAbout), &frmAbout);
		Application->CreateForm(__classid(TfrmLog), &frmLog);
		Application->CreateForm(__classid(TfrmResizeCfg), &frmResizeCfg);
		Application->CreateForm(__classid(TfrmRename), &frmRename);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
