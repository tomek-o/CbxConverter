//---------------------------------------------------------------------------


#pragma hdrstop

#include <IniFiles.hpp>
#include "Settings.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

Settings appSettings;

void Settings::SetDefault(void)
{
	Directories.tmpDirectoryType = _Directories::DirectoryTypeDefault;
	Directories.customTmpDirectory = "";
	Directories.outDirectoryType = _Directories::DirectoryTypeDefault;
	Directories.customOutDirectory = "";
	Directories.useSourceDirectoryForOutput = false;
	Directories.recreateSourceDirectoryForOutput = false;
	Directories.useNumericTmpFileDirectory = false;

    Conversion.copyFileToOutputOnError = false;

	PdfImport.sGsParams = "-sDEVICE=png16m -r300x300";
	PdfImport.sGsFilePattern = "%03d.png";
}

bool Settings::Read(AnsiString asFileName)
{
	SetDefault();
	TIniFile *ini = NULL;
	try
	{
		ini = new TIniFile(asFileName);
		// main window startup position
		int maxX = GetSystemMetrics(SM_CXSCREEN);
		int maxY = GetSystemMetrics(SM_CYSCREEN);
		frmMain.iWidth = ini->ReadInteger("frmMain", "AppWidth", 950);
		frmMain.iHeight = ini->ReadInteger("frmMain", "AppHeight", 400);
		if (frmMain.iWidth < 250 || frmMain.iWidth > maxX + 20)
		{
			frmMain.iWidth = 250;
		}
		if (frmMain.iHeight < 200 || frmMain.iHeight > maxY + 20)
		{
			frmMain.iHeight = 200;
		}
		frmMain.iPosX = ini->ReadInteger("frmMain", "AppPositionX", 30);
		frmMain.iPosY = ini->ReadInteger("frmMain", "AppPositionY", 30);
		if (frmMain.iPosX < 0)
			frmMain.iPosX = 0;
		if (frmMain.iPosY < 0)
			frmMain.iPosY = 0;
		if (frmMain.iPosX + frmMain.iWidth > maxX)
			frmMain.iPosX = maxX - frmMain.iWidth;
		if (frmMain.iPosY + frmMain.iHeight > maxY)
			frmMain.iPosY = maxY - frmMain.iHeight;
		frmMain.bWindowMaximized = ini->ReadBool("frmMain", "Maximized", false);
		frmMain.bAlwaysOnTop = ini->ReadBool("frmMain", "AlwaysOnTop", false);

		Logging.bLogToFile = ini->ReadBool("Logging", "LogToFile", false);
		Logging.iMaxUiLogLines = ini->ReadInteger("Logging", "MaxUiLogLines", 1000);

		Worker.priority = ini->ReadInteger("Worker", "Priority", BELOW_NORMAL_PRIORITY_CLASS);

		if (Worker.priority != IDLE_PRIORITY_CLASS &&
			Worker.priority != BELOW_NORMAL_PRIORITY_CLASS &&
			Worker.priority != NORMAL_PRIORITY_CLASS &&
			Worker.priority != ABOVE_NORMAL_PRIORITY_CLASS)
		{
			Worker.priority = BELOW_NORMAL_PRIORITY_CLASS;
		}

		Worker.playSoundWhenDone = ini->ReadBool("Worker", "PlaySoundWhenDone", true);
		Worker.threadCount = ini->ReadInteger("Worker", "ThreadCount", _Worker::THREAD_COUNT_DEF);
		if (Worker.threadCount < _Worker::THREAD_COUNT_MIN || Worker.threadCount > _Worker::THREAD_COUNT_MAX)
			Worker.threadCount = _Worker::THREAD_COUNT_DEF;

		for (unsigned int i=0; i<sizeof(Conversion.DefResize)/sizeof(Conversion.DefResize[0]); i++)
		{
			AnsiString key1, key2;
			key1.sprintf("DefaultResizeWidth%d", i);
			key2.sprintf("DefaultResizePct%d", i);
			Conversion.DefResize[i].width = ini->ReadInteger("Conversion", key1, 0);
			Conversion.DefResize[i].resize = ini->ReadInteger("Conversion", key2, 100);
			if (Conversion.DefResize[i].resize <= 0 || Conversion.DefResize[i].resize >= 1000)
			{
				Conversion.DefResize[i].resize = 0;
			}
		}
		Conversion.sImExtraParams = ini->ReadString("Conversion", "ImExtraParams", "-quality 75");
		Conversion.sImExtraParamsBeforeResize = ini->ReadString("Conversion", "ImExtraParamsBeforeResize", "");
		Conversion.sOutputExtension = ini->ReadString("Conversion", "OutputExtension", "webp");
		Conversion.filesToSkip = ini->ReadString("Conversion", "FilesToSkip", "");
		Conversion.unpackPassword = ini->ReadString("Conversion", "UnpackPassword", "");
		Conversion.copyFileToOutputOnError = ini->ReadBool("Conversion", "copyFileToOutputOnError", Conversion.copyFileToOutputOnError);

		PdfImport.sGsLocation = ini->ReadString("PdfImport", "GsLocation", PdfImport.sGsLocation);
		PdfImport.sGsParams = ini->ReadString("PdfImport", "GsParams", PdfImport.sGsParams);
		PdfImport.sGsFilePattern = ini->ReadString("PdfImport", "GsFilePattern", PdfImport.sGsFilePattern);

		Directories.tmpDirectoryType = static_cast<_Directories::DirectoryType>(ini->ReadInteger("Directories", "TmpDirectoryType", Directories.tmpDirectoryType));
		if (Directories.tmpDirectoryType < 0 || Directories.tmpDirectoryType >= _Directories::DirectoryTypeLimiter)
			Directories.tmpDirectoryType = _Directories::DirectoryTypeDefault;
		Directories.customTmpDirectory = ini->ReadString("Directories", "CustomTmpDirectory", Directories.customTmpDirectory);
		Directories.outDirectoryType = static_cast<_Directories::DirectoryType>(ini->ReadInteger("Directories", "OutDirectoryType", Directories.outDirectoryType));
		if (Directories.outDirectoryType < 0 || Directories.outDirectoryType >= _Directories::DirectoryTypeLimiter)
			Directories.outDirectoryType = _Directories::DirectoryTypeDefault;
		Directories.customOutDirectory = ini->ReadString("Directories", "CustomOutDirectory", Directories.customOutDirectory);

		Directories.useSourceDirectoryForOutput = ini->ReadBool("Directories", "UseSourceDirectoryForOutput", Directories.useSourceDirectoryForOutput);
		Directories.recreateSourceDirectoryForOutput = ini->ReadBool("Directories", "RecreateSourceDirectoryForOutput", Directories.recreateSourceDirectoryForOutput);
		Directories.useNumericTmpFileDirectory = ini->ReadBool("Directories", "UseNumericTmpFileDirectory", Directories.useNumericTmpFileDirectory);

		delete ini;
	}
	catch (...)
	{
		if (ini)
		{
			delete ini;
		}
		return false;
	}
	return true;
}

bool Settings::Write(AnsiString asFileName)
{
	TIniFile *ini = NULL;
	try
	{
		ini = new TIniFile(asFileName);
		ini->WriteInteger("frmMain", "AppWidth", frmMain.iWidth);
		ini->WriteInteger("frmMain", "AppHeight", frmMain.iHeight);
		ini->WriteInteger("frmMain", "AppPositionX", frmMain.iPosX);
		ini->WriteInteger("frmMain", "AppPositionY", frmMain.iPosY);
		ini->WriteBool("frmMain", "Maximized", frmMain.bWindowMaximized);
		ini->WriteBool("frmMain", "AlwaysOnTop", frmMain.bAlwaysOnTop);

		ini->WriteBool("Logging", "LogToFile", Logging.bLogToFile);
		ini->WriteInteger("Logging", "MaxUiLogLines", Logging.iMaxUiLogLines);

        ini->WriteInteger("Worker", "Priority", Worker.priority);
		ini->WriteBool("Worker", "PlaySoundWhenDone", Worker.playSoundWhenDone);
		ini->WriteInteger("Worker", "ThreadCount", Worker.threadCount);

		for (unsigned int i=0; i<sizeof(Conversion.DefResize)/sizeof(Conversion.DefResize[0]); i++)
		{
			AnsiString key1, key2;
			key1.sprintf("DefaultResizeWidth%d", i);
			key2.sprintf("DefaultResizePct%d", i);
			ini->WriteInteger("Conversion", key1, Conversion.DefResize[i].width);
			if (Conversion.DefResize[i].resize <= 0 || Conversion.DefResize[i].resize >= 1000)
			{
				Conversion.DefResize[i].resize = 0;
			}
			ini->WriteInteger("Conversion", key2, Conversion.DefResize[i].resize);
		}
		ini->WriteString("Conversion", "ImExtraParams", Conversion.sImExtraParams);
		ini->WriteString("Conversion", "ImExtraParamsBeforeResize", Conversion.sImExtraParamsBeforeResize);
		ini->WriteString("Conversion", "OutputExtension", Conversion.sOutputExtension);
		ini->WriteString("Conversion", "FilesToSkip", Conversion.filesToSkip);
		ini->WriteString("Conversion", "UnpackPassword", Conversion.unpackPassword);
		ini->WriteBool("Conversion", "copyFileToOutputOnError", Conversion.copyFileToOutputOnError);

		ini->WriteString("PdfImport", "GsLocation", PdfImport.sGsLocation);
		ini->WriteString("PdfImport", "GsParams", PdfImport.sGsParams);
		ini->WriteString("PdfImport", "GsFilePattern", PdfImport.sGsFilePattern);

		ini->WriteInteger("Directories", "TmpDirectoryType", Directories.tmpDirectoryType);
		ini->WriteString("Directories", "CustomTmpDirectory", Directories.customTmpDirectory);
		ini->WriteInteger("Directories", "OutDirectoryType", Directories.outDirectoryType);
		ini->WriteString("Directories", "CustomOutDirectory", Directories.customOutDirectory);

		ini->WriteBool("Directories", "UseSourceDirectoryForOutput", Directories.useSourceDirectoryForOutput);
		ini->WriteBool("Directories", "RecreateSourceDirectoryForOutput", Directories.recreateSourceDirectoryForOutput);
		ini->WriteBool("Directories", "UseNumericTmpFileDirectory", Directories.useNumericTmpFileDirectory);
				
		delete ini;
	}
	catch (...)
	{
		if (ini)
		{
			delete ini;
		}
		return false;
	}
	return true;

}

const char* Settings::_Directories::DirectoryTypeDescription(enum Settings::_Directories::DirectoryType type)
{
	switch (type)
	{
		case _Directories::DirectoryTypeDefault:
			return "default (relative subdirectory)";
		case _Directories::DirectoryTypeCustom:
			return "custom";
		default:
			return "???";
    }
}
