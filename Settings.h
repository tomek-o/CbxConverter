/** \file
*/
//---------------------------------------------------------------------------

#ifndef SettingsH
#define SettingsH
//---------------------------------------------------------------------------
#include <System.hpp>

class Settings
{
public:
	bool Read(AnsiString asFileName);
	bool Write(AnsiString asFileName);
	struct _frmMain
	{
		int iPosX, iPosY;				///< main window coordinates
		int iHeight, iWidth;			///< main window size
		bool bWindowMaximized;			///< is main window maximize?
		bool bAlwaysOnTop;
	} frmMain;
	struct _Logging
	{
		bool bLogToFile;
		unsigned int iMaxUiLogLines;
	} Logging;
	struct _Worker
	{
		int priority;
		bool playSoundWhenDone;
		enum
		{
			THREAD_COUNT_MIN = 1,
			THREAD_COUNT_MAX = 16
		};
		enum
		{
        	THREAD_COUNT_DEF = 1
		};
		int threadCount;
	} Worker;
	struct _Conversion
	{
        AnsiString unpackPassword;
		struct _DefResize
		{
			int width;
			int resize;
		} DefResize[10];
		AnsiString sImExtraParamsBeforeResize;
		AnsiString sImExtraParams;
		AnsiString sOutputExtension;
		AnsiString filesToSkip;			// multiple DOS-like file masks, separated with semicolon if more than one is specified
		bool copyFileToOutputOnError;		
	} Conversion;
	struct _PdfImport
	{
		AnsiString sGsLocation;
		AnsiString sGsParams;
		AnsiString sGsFilePattern;
	} PdfImport;
	struct _Directories
	{
		enum DirectoryType
		{
			DirectoryTypeDefault = 0,
			DirectoryTypeCustom,
			
			DirectoryTypeLimiter
		};
		static const char* DirectoryTypeDescription(enum DirectoryType type);
		enum DirectoryType tmpDirectoryType;
		AnsiString customTmpDirectory;
		enum DirectoryType outDirectoryType;
		AnsiString customOutDirectory;

		bool useSourceDirectoryForOutput;
		bool recreateSourceDirectoryForOutput;
		bool useNumericTmpFileDirectory;	///< use number (e.g. "0000") instead of full file name for temporary directory to shorten path		
	} Directories;

private:
	void SetDefault(void);
};
extern Settings appSettings;

#endif
