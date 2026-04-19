//---------------------------------------------------------------------------


#pragma hdrstop
#include "Job.h"
#include "Utils.h"
#include "Log.h"
#include "SourceFile.h"
#include "Image.h"
#include "Settings.h"
#include "FileNameMatch.h"
#include <sys\stat.h>
#include <assert.h>
#include <System.hpp>
#include <Forms.hpp>
#include <jpeg.hpp>	///< for basic JPEG size info
#include <algorithm>
#include <iterator>

#pragma warn -8091	// incorrectly issued by BDS2006

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace
{
	AnsiString appPath;
	AnsiString tmpPath;
	AnsiString outPath;
	bool useSourceDirectoryForOutput = false;
	bool recreateSourceDirectoryForOutput = false;
	AnsiString zipPath;
	AnsiString imPath;	// ImageMagick
	AnsiString gsPath;	// GhostScript
	AnsiString gsParams;
	AnsiString gsFilePattern;
	bool Init(void)
	{
		appPath = ExtractFilePath(Application->ExeName);
		tmpPath = appPath + "tmp\\";
		zipPath = appPath + "7z.exe";
		imPath = appPath + "magick.exe";
		outPath = appPath + "out\\";
		if (ForceDirectories(tmpPath) == false)
		{
			LOG("ERROR: Failed to create temporary directory!");
			return false;
		}
		if (ForceDirectories(outPath) == false)
		{
			LOG("ERROR: Failed to create output directory!");
			return false;
		}
		return true;
	}
	bool initialized = Init();

	struct FileNameSize
	{
		AnsiString name;
		uint64_t size;
	};

	void ScanDir(AnsiString path, std::vector<FileNameSize> &files)
	{
		int iAttributes = faAnyFile;
		Sysutils::TSearchRec sr;
		if (FindFirst(path, iAttributes, sr) == 0)
		{
			do
			{
				// drill into subdirs
				if (sr.Attr & faDirectory) {
					// Do not drill into "." or ".."
					if (sr.Name != "." && sr.Name != "..") {
						if(path.Length() + sr.Name.Length() < MAX_PATH-1)
						{
							AnsiString drillPath = appendDirectory(path, sr.Name);
							ScanDir(drillPath, files);
						}
					}
				}
				else
				{
					FileNameSize f;
					f.name = ExtractFilePath(path) + sr.Name;
					f.size = sr.Size;
					files.push_back(f);
				}
			} while (FindNext(sr) == 0);
			FindClose(sr);
		}
	}
}

AnsiString Job::GetOutPath(void)
{
	return outPath;
}

AnsiString Job::GetTmpPath(void)
{
	return tmpPath;
}


Job::Job(SourceFile *file):
	type(INVALID),
	file(file)
{
	if (file)
	{
		tmpDir = file->tmpDir;
		rootDir = file->rootPath;
	}
}

const char* Job::GetTypeName(void) const
{
	switch (type)
	{
		case INVALID:
			return "INVALID";
		case UNPACK:
			return "UNPACK";
		case RENDER_PDF:
			return "RENDER_PDF";
		case CONVERT:
			return "CONVERT";
		default:
			return "???";
	}
}

void Job::SetTmpPath(AnsiString dir)
{
	if (dir[dir.Length()] != '\\')
		dir += "\\";
	tmpPath = dir;
	if (ForceDirectories(tmpPath) == false)
	{
		LOG("ERROR: Failed to create temporary directory!");
	}
}

void Job::SetOutPath(AnsiString dir)
{
	if (dir[dir.Length()] != '\\')
		dir += "\\";
	outPath = dir;
	if (ForceDirectories(outPath) == false)
	{
		LOG("ERROR: Failed to create output directory!");
	}
}

void Job::SetUseSourceDirectoryForOutput(bool val)
{
	useSourceDirectoryForOutput = val;
}

void Job::SetRecreateSourceDirectoryForOutput(bool val)
{
	recreateSourceDirectoryForOutput = val;
}

void Job::SetGsPath(AnsiString gsExecutable)
{
    gsPath = gsExecutable;
}

void Job::SetGsParams(AnsiString params)
{
	gsParams = params;
}

void Job::SetGsFilePattern(AnsiString pattern)
{
    gsFilePattern = pattern;
}

void Job::Execute(bool *abort, int priority)
{
	assert(abort);
	this->abort = abort;
	if (file->state != SourceFile::DONE)
	{
		//LOG("Executing job type = %d, file = %s", (int)type, fileName.c_str());
		switch (type)
		{
		case UNPACK:
			file->state = SourceFile::UNPACKING;
			Unpack();
			break;
		case RENDER_PDF:
			file->state = SourceFile::RENDERING_PDF;
			RenderPdf();
			break;
		case CONVERT:
			if (file->unpacked)
			{
				file->state = SourceFile::CONVERTING;
				Convert();
				if (!file->doNotPack)
				{
					file->state = SourceFile::PACKING;
					Pack();
				}
				else
				{
					file->state = SourceFile::DONE;
				}
			}
			break;
		default:
			assert(!"Unhandled job type!");
		}
		file->jobCount--;
		if (file->state != SourceFile::DONE && file->state != SourceFile::SOURCE_ERROR)
		{
			file->state = SourceFile::IDLE;
		}

		if ((file->state == SourceFile::SOURCE_ERROR)
			&& !useSourceDirectoryForOutput
			&& appSettings.Conversion.copyFileToOutputOnError)
		{
        	Copy();
		}
	}
}

bool compareWidthFreq(std::pair<int, int> &a, std::pair<int, int> &b)
{
	return a.second > b.second;
}

void Job::Unpack(void)
{
	AnsiString cmdline;
	AnsiString dir = tmpDir;

	// remove existing directory if exists
	if (DirectoryExists(dir))
	{
		AnsiString d = dir.SubString(1, dir.Length() - 1);
		DeleteDirectory(NULL, d, true);
	}

	if (ForceDirectories(dir) == false)
	{
		LOG("Failed to create tmp dir %s", dir.c_str());
		file->state = SourceFile::SOURCE_ERROR;		
		return;
	}
	cmdline.sprintf("\"%s\" -p\"%s\" x \"%s\" -y -o\"%s\" ",
		zipPath.c_str(), appSettings.Conversion.unpackPassword.c_str(), fileName.c_str(), dir.c_str());

	ProcessSource(dir, cmdline);
}

void Job::RenderPdf(void)
{
	AnsiString cmdline;
	AnsiString dir = tmpDir;

	// remove existing directory if exists
	if (DirectoryExists(dir))
	{
		AnsiString d = dir.SubString(1, dir.Length() - 1);
		DeleteDirectory(NULL, d, true);
	}

	if (ForceDirectories(dir) == false)
	{
		LOG("Failed to create tmp dir %s", dir.c_str());
		file->state = SourceFile::SOURCE_ERROR;		
		return;
	}
	cmdline.sprintf("\"%s\" -q -dBATCH -dNOPAUSE %s -sOutputFile=\"%s%s\" \"%s\" ",
		gsPath.c_str(), gsParams.c_str(), dir.c_str(), gsFilePattern.c_str(), fileName.c_str());

	ProcessSource(dir, cmdline);
}

void Job::ProcessSource(AnsiString dir, AnsiString cmdline)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	// Start the child process.
	if( !CreateProcess( NULL, // No module name (use command line)
		cmdline.c_str(), // Command line
		NULL, // Process handle not inheritable
		NULL, // Thread handle not inheritable
		FALSE, // Set handle inheritance to FALSE
		appSettings.Worker.priority,
		NULL, // Use parent's environment block
		NULL, // Use parent's starting directory
		&si, // Pointer to STARTUPINFO structure
		&pi ) // Pointer to PROCESS_INFORMATION structure
		)
	{
		LOG( "CreateProcess failed (%d)", GetLastError() );
		return;
	}

	// Wait until child process exits.
	WaitForSingleObject( pi.hProcess, INFINITE );

	DWORD exitCode;
	BOOL result = GetExitCodeProcess(pi.hProcess, &exitCode);	

	// Close process and thread handles.
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );

	//LOG("Unpack status (%s) = %d", fileName.c_str(), status);
	if (result)
	{
		if (exitCode == 0)
		{

		}
		else
		{
			LOG("Error running [%s]", cmdline.c_str());
			file->state = SourceFile::SOURCE_ERROR;
			return;
		}
	}
	else
	{
		LOG("Could not get status code from [%s]", cmdline.c_str());
		file->state = SourceFile::SOURCE_ERROR;		
		return;
	}

	file->stateProgress = 0;
	file->state = SourceFile::IDENTIFYING;
	file->fileDescs.clear();
	dir = tmpDir + "*.*";
	std::vector<FileNameSize> files;
	ScanDir(dir, files);

	for (unsigned int i=0; i<files.size(); i++)
	{
		AnsiString name = files[i].name;
		uint64_t fsize = files[i].size;
		SourceFile::FileDesc desc;
		desc.name = name;
		desc.size = fsize;

        file->stateProgress = 100 * i / files.size();

		int width = 0, height = 0;
		if (Image::Identify(name, width, height) == 0)
		{
			file->imgCount++;
			file->imgSize += fsize;
			file->widths[width]++;
			desc.width = width;
			desc.height = height;
		}
		file->fileDescs.push_back(desc);
	}

	// report img widths;
	std::vector<std::pair<int, int> > v;
	std::copy(file->widths.begin(), file->widths.end(),
		std::back_inserter<std::vector<std::pair<int, int> > >(v));
	std::sort(v.begin(), v.end(), compareWidthFreq);

	if (v.size() == 0)
	{
		file->asWidth = "?";
	}
	else if (v.size() == 1)
	{
		file->asWidth.sprintf("%dpx", v[0].first);
		file->mostFrequentWidth = v[0].first;
	}
	else
	{
		file->asWidth.sprintf("%dpx (%d), %dpx (%d)",
			v[0].first, v[0].second,
			v[1].first, v[1].second);
		file->mostFrequentWidth = v[0].first;
	}

	file->widthValid = true;

	file->unpacked = true;
	LOG("Archive content: %u files", file->fileDescs.size());
}

void Job::Convert(void)
{
	AnsiString filesToSkip = appSettings.Conversion.filesToSkip.Trim();
	file->stateProgress = 0;
	for (unsigned int i=0; i<file->fileDescs.size(); i++)
	{
		const struct SourceFile::FileDesc &fd = file->fileDescs[i];
		AnsiString fname = ExtractFileName(fd.name);
		if (filesToSkip != "")
		{
			if (MatchMultiplePatterns(fname, filesToSkip))
			{
				LOG("Skipping conversion of %s", fname.c_str());
				continue;
			}
		}
		if (*abort)
			break;
		file->stateProgress = 100 * i / file->fileDescs.size();
		ConvertFile(fd.name, fd.width, fd.height);
	}
}

void Job::ConvertFile(AnsiString path, int width, int height)
{
	AnsiString cmdline;
	AnsiString extension = sOutputExtension;
	if (extension[1] != '.')
		extension = (AnsiString)"." + extension;
	AnsiString out = ChangeFileExt(path, extension);
	int resizePct = 100;
	AnsiString resizeCmd;

	if (resizeCfg.mode == ResizeCfg::ModeRegular)
	{
		resizePct = resizeCfg.resizePct;
		resizeCmd.sprintf("-resize %d%%", resizePct);
	}
	else if (resizeCfg.mode == ResizeCfg::ModeOnOversize)
	{
		resizeCmd.sprintf("-resize %d%%", resizePct);
		if (width > 0 && height > 0)
		{
			int maxDimension = 0;
			switch (resizeCfg.modeOnOversize)
			{
			case ResizeCfg::ModeOnOversizeWiderOrHigher:
				maxDimension = std::max(width, height);
				break;
			case ResizeCfg::ModeOnOversizeWider:
				maxDimension = width;
				break;
			case ResizeCfg::ModeOnOversizeHigher:
				maxDimension = height;
				break;
			default:
				assert(!"Unhandled modeOnOversize!");
			}
			if (maxDimension >= resizeCfg.resizeThreshold)
			{
				float scaling = (float)resizeCfg.resizeTarget/(float)maxDimension;
				width = (int)(scaling * width + 0.5f);
				height = (int)(scaling * height + 0.5f);
				resizeCmd.sprintf("-resize %dx%d", width, height);
            }
        }
	}
	else
	{
		assert(!"Unhandled resize mode");
	}
#if 1
	cmdline.sprintf("\"%s\" \"%s\" %s %s %s \"%s\" ",
		imPath.c_str(), path.c_str(),
		sImExtraParamsBeforeResize.c_str(), resizeCmd.c_str(), sImExtraParams.c_str(),
		out.c_str());
#else
	AnsiString currentDirectory = ExtractFilePath(path);
	SetCurrentDirectory(currentDirectory.c_str());
	AnsiString sourceFileName = ExtractFileName(path);
	AnsiString outFileName = ExtractFileName(out);
	cmdline.sprintf("\"%s\" \"%s\" %s %s %s \"%s\" ",
		imPath.c_str(), sourceFileName.c_str(),
		sImExtraParamsBeforeResize.c_str(), resizeCmd.c_str(), sImExtraParams.c_str(),
		outFileName.c_str());
	LOG("cmd = %s", cmdline.c_str());
#endif
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
    ZeroMemory( &si, sizeof(si) ); 
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	// Start the child process.
	if( !CreateProcess( NULL, // No module name (use command line)
		cmdline.c_str(), // Command line
		NULL, // Process handle not inheritable
		NULL, // Thread handle not inheritable
		FALSE, // Set handle inheritance to FALSE
		appSettings.Worker.priority,
		NULL, // Use parent's environment block
		NULL, // NULL = Use parent's starting directory
		&si, // Pointer to STARTUPINFO structure
		&pi ) // Pointer to PROCESS_INFORMATION structure
		)
	{
		LOG( "CreateProcess failed (%d)", GetLastError() );
		return;
	}

	// Wait until child process exits.
	WaitForSingleObject( pi.hProcess, INFINITE );

	DWORD exitCode;
	BOOL result = GetExitCodeProcess(pi.hProcess, &exitCode);

	// Close process and thread handles.
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );

	if (result)
	{
		if (exitCode == 0)
		{
		#if 0
			// could not delete read-only
			if (DeleteFile(path) == false)
			{
				LOG("Error deleting %s", path.c_str());
			}
		#else
			if (UpperCase(path) != UpperCase(out))
			{
				DeleteDirectory(NULL, path, true);
			}
		#endif
		}
		else
		{
			LOG("Error converting %s", path.c_str());
		}
	}
	else
	{
		LOG("Could not get status code from converter");
	}
}

void Job::Pack(void)
{
	AnsiString src = tmpDir;

	AnsiString cmdline;
	AnsiString outFile = ChangeFileExt(ExtractFileName(fileName), ".cbz");

	AnsiString path = GetOutFilePath();

	if (useSourceDirectoryForOutput)
	{
		AnsiString extension = sOutputExtension;
		if (extension[1] != '.')
			extension = (AnsiString)"." + extension;
		outFile = ChangeFileExt(outFile, extension + ".cbz");
	}

	file->outFile = path + outFile;

	cmdline.sprintf("\"%s\" a -tzip \"%s%s\" \"%s*.*\" -r -mx0",
		zipPath.c_str(),
		path.c_str(), outFile.c_str(),
		src.c_str()
		);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
    ZeroMemory( &si, sizeof(si) ); 
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	// Start the child process.
	if( !CreateProcess( NULL, // No module name (use command line)
		cmdline.c_str(), // Command line
		NULL, // Process handle not inheritable
		NULL, // Thread handle not inheritable
		FALSE, // Set handle inheritance to FALSE
		appSettings.Worker.priority,
		NULL, // Use parent's environment block
		NULL, // Use parent's starting directory
		&si, // Pointer to STARTUPINFO structure
		&pi ) // Pointer to PROCESS_INFORMATION structure
		)
	{
		LOG( "CreateProcess failed (%d)", GetLastError() );
		return;
	}

	// Wait until child process exits.
	WaitForSingleObject( pi.hProcess, INFINITE );

	DWORD exitCode;
	BOOL result = GetExitCodeProcess(pi.hProcess, &exitCode);

	// Close process and thread handles.
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );

	if (result)
	{
		if (exitCode == 0)
		{
            // remove trailing backslash
			AnsiString d = src.SubString(1, src.Length() - 1);
			DeleteDirectory(NULL, d, true);
			file->state = SourceFile::DONE;
			struct stati64 statbuf;
			AnsiString name = path + outFile;
			_stati64(name.c_str(), &statbuf);
			file->outSize = statbuf.st_size;
			file->outSizeValid = true;
		}
		else
		{
			LOG("Error packing back %s", src.c_str());
			LOG("Command = %s", cmdline.c_str());
		}
	}
	else
	{
		LOG("Could not get status code from 7z");
	}
}

void Job::Copy(void)
{
	AnsiString outFile = ExtractFileName(fileName);
	file->outFile = GetOutFilePath() + outFile;
	CopyFile(fileName.c_str(), file->outFile.c_str(), 0);
}

AnsiString Job::GetOutFilePath(void)
{
	AnsiString path = outPath;
	
	if (useSourceDirectoryForOutput)
	{
		path = ExtractFileDir(fileName) + "\\";
	}
	else if (recreateSourceDirectoryForOutput)
	{
		if (rootDir != "")
		{
			AnsiString fileDir = ExtractFileDir(fileName);
			AnsiString delta = fileDir.SubString(rootDir.Length()+1, fileDir.Length()-rootDir.Length());
			if (delta[1] == '\\')
				delta = delta.SubString(2, delta.Length()-1);
			if (delta != "")
			{
				path = path + delta + "\\";
				if (DirectoryExists(path) == false)
				{
					if (ForceDirectories(path) == false)
					{
						LOG("ERROR: Failed to create output directory (%s)!", path.c_str());
					}
				}
			}
		}
	}
	
	return path;
}

