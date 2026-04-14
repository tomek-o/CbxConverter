//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Image.h"
#include "Settings.h"
#include "Log.h"
#include <SysUtils.hpp>
#include <stdio.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace
{

AnsiString appPath;
AnsiString imPath;	// ImageMagick
bool Init(void)
{
	appPath = ExtractFilePath(Application->ExeName);
	imPath = appPath + "magick.exe";
	return true;
}
bool initialized = Init();

int CreateProcessAndCaptureOutput(AnsiString exeName, AnsiString params, AnsiString &output)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	SECURITY_ATTRIBUTES sa;
	HANDLE hReadPipe, hWritePipe;
	char Buffer[4000];
	DWORD dwRead;

	sa.nLength = sizeof (SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = true;

	int rc;

	rc = CreatePipe(&hReadPipe, &hWritePipe, &sa, 2500000);
	if (!rc)
	{
		LOG("Could not identify image: failed to create pipe");
		return -1;
	}

	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof (STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	si.hStdOutput = hWritePipe;

	AnsiString cmdLine = exeName + " " + params;
	rc = CreateProcess(NULL, cmdLine.c_str(), NULL, NULL, true, NULL, NULL, NULL, &si, &pi);
	if (!rc)
	{
		LOG("Could not identify image: could not create ImageMagick process");
		CloseHandle(hReadPipe);
		CloseHandle(hWritePipe);
		return -2;
	}

	// Wait until child process exits.
	WaitForSingleObject( pi.hProcess, INFINITE );

	DWORD exitCode = -1;
	BOOL result = GetExitCodeProcess(pi.hProcess, &exitCode);

	// Close process and thread handles.
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );

	if (result)
	{
		if (exitCode == 0)
		{
		}
		else
		{
			LOG("Error running %s, exitCode = %d", imPath.c_str(), exitCode);
		}
	}
	else
	{
		LOG("Could not get status code from magick.exe");
	}

	// closing not needed end of the pipe
	// otherwise ReadFile would stuck infinitely
	// https://stackoverflow.com/questions/35969730/how-to-read-output-from-cmd-exe-using-createprocess-and-createpipe
	CloseHandle(hWritePipe);

	memset(Buffer, 0, 4000);
	rc = ReadFile(hReadPipe, &Buffer, sizeof(Buffer)-1, &dwRead, NULL);
	if (!rc)
	{
		LOG("Could not identify Image: could not read ImageMagick output");
		CloseHandle(hReadPipe);
		return -3;
	}
	output = Buffer;
	CloseHandle(hReadPipe);

	return 0;
}

}	// anonymous namespace

int Image::Identify(AnsiString fileName, int& width, int& height)
{
	AnsiString params;
	params.sprintf("identify -format \"%%[w] %%[h]\" \"%s\"", fileName.c_str());
	AnsiString output;
	int rc = CreateProcessAndCaptureOutput(imPath, params, output);
	/*
		-> magick identify rose.jpg
		rose.jpg JPEG 70x46 70x46+0+0 8-bit sRGB 2.36KB 0.000u 0:00.000
	*/
	if (rc)
	{
        LOG("Could not identify %s\n", fileName.c_str());
		return rc;
	}

	if (sscanf(output.c_str(), "%d %d", &width, &height) != 2)
	{
		LOG("Could not parse magick identify output for file %s: [%s]\n", fileName.c_str(), output.c_str());
		return -1;
    }

	return 0;
}

