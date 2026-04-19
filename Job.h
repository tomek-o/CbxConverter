//---------------------------------------------------------------------------

#ifndef JobH
#define JobH
//---------------------------------------------------------------------------

#include <Classes.hpp>
#include <SysUtils.hpp>
#include <vector>

#include "ResizeCfg.h"
#include "SourceFile.h"

class Job
{
public:
	enum Type
	{
		INVALID = 0,
		UNPACK,
		RENDER_PDF,
		CONVERT
	} type;
	AnsiString fileName;
	AnsiString tmpDir;
	AnsiString rootDir;
	ResizeCfg resizeCfg;
	AnsiString sImExtraParamsBeforeResize;
	AnsiString sImExtraParams;
	AnsiString sOutputExtension;

	void Execute(bool *abort, int priority);

	static void SetTmpPath(AnsiString dir);
	static void SetOutPath(AnsiString dir);
	static void SetUseSourceDirectoryForOutput(bool val);
	static void SetRecreateSourceDirectoryForOutput(bool val);
	static void SetGsPath(AnsiString gsExecutable);
	static void SetGsParams(AnsiString params);
	static void SetGsFilePattern(AnsiString pattern);

	static AnsiString GetOutPath(void);
	static AnsiString GetTmpPath(void);

	SourceFile *file;
	Job(SourceFile *file);
	const char* GetTypeName(void) const;

private:
	void Unpack(void);
	void RenderPdf(void);
	void Convert(void);
	void ConvertFile(struct SourceFile::FileDesc &fd);
	void Pack(void);
	void Copy(void);

	void ProcessSource(AnsiString dir, AnsiString cmdline);
	AnsiString GetOutFilePath(void);


	bool* abort;
};

#endif
