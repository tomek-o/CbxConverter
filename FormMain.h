//---------------------------------------------------------------------------

#ifndef FormMainH
#define FormMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include "SourceFile.h"
#include <Menus.hpp>
#include <ActnList.hpp>
#include <ImgList.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>

#include <vector>
#include <list>

//---------------------------------------------------------------------------
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
	TMainMenu *MainMenu;
	TMenuItem *miFile;
	TMenuItem *miSettings;
	TMenuItem *miHelp;
	TMenuItem *miAbout;
	TMenuItem *miCommonSettings;
	TActionList *ActionList;
	TImageList *imglistActions;
	TAction *actShowAbout;
	TAction *actShowSettings;
	TStatusBar *StatusBar;
	TMenuItem *miView;
	TMenuItem *miViewLog;
	TAction *actShowLog;
	TPanel *pnlBottom;
	TListView *lvInput;
	TMenuItem *miAddSourceFile;
	TOpenDialog *dlgOpen;
	TButton *btnStartConversion;
	TTimer *tmrRefreshStateView;
	TPopupMenu *popupInput;
	TMenuItem *miResize;
	TMenuItem *miClearConverted;
	TMenuItem *N1;
	TMenuItem *miRemoveFile;
	TTimer *tmrStartup;
	TMenuItem *miExit;
	TMenuItem *miDeleteSourceFiles;
	TMenuItem *N2;
	TMenuItem *N3;
	TMenuItem *miOpenTmpDirectory;
	TMenuItem *miOpenOutputDirectory;
	TMenuItem *miAddRecursivelyFilesFromDirectory;
	TTimer *tmrJobQueuePoll;
	TMenuItem *miDeleteOutputFiles;
	TImageList *imagelistInput;
	TMenuItem *N4;
	TMenuItem *miSelectAll;
	TMenuItem *miRenameImageFiles;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall actShowAboutExecute(TObject *Sender);
	void __fastcall actShowSettingsExecute(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall actShowLogExecute(TObject *Sender);
	void __fastcall miAddSourceFileClick(TObject *Sender);
	void __fastcall lvInputData(TObject *Sender, TListItem *Item);
	void __fastcall btnStartConversionClick(TObject *Sender);
	void __fastcall tmrRefreshStateViewTimer(TObject *Sender);
	void __fastcall miResizeClick(TObject *Sender);
	void __fastcall miClearConvertedClick(TObject *Sender);
	void __fastcall miRemoveFileClick(TObject *Sender);
	void __fastcall tmrStartupTimer(TObject *Sender);
	void __fastcall miExitClick(TObject *Sender);
	void __fastcall miDeleteSourceFilesClick(TObject *Sender);
	void __fastcall miOpenTmpDirectoryClick(TObject *Sender);
	void __fastcall miOpenOutputDirectoryClick(TObject *Sender);
	void __fastcall miAddRecursivelyFilesFromDirectoryClick(TObject *Sender);
	void __fastcall tmrJobQueuePollTimer(TObject *Sender);
	void __fastcall miDeleteOutputFilesClick(TObject *Sender);
	void __fastcall lvInputColumnClick(TObject *Sender, TListColumn *Column);
	void __fastcall miSelectAllClick(TObject *Sender);
	void __fastcall miRenameImageFilesClick(TObject *Sender);
private:	// User declarations
	void AddSourceFile(AnsiString name, AnsiString rootPath);
	/** \brief Add all files from the folder, recursively */
	void AddSourceDir(AnsiString path, AnsiString rootPath);
	void __fastcall WMDropFiles(TWMDropFiles &message);
	enum SortType {
		SortByFileName = 0,
		SortBySize,
		SortByImageCnt,
		SortByImageSize,
		SortByImageWidth,
		SortByOutSize,
		SortByOutSizeRatio,

		SortTypeLimiter
	};
	int sort(enum SortType type, bool ascending);
	std::vector<SourceFile*> sourceFiles;
	std::list<SourceFile> sourceFilesList;
	void UpdateSourceView(void);
	int GetDefaultResize(int width);
	void ApplySettings(void);
public:		// User declarations
	__fastcall TfrmMain(TComponent* Owner);
	__fastcall ~TfrmMain(void);
	static AnsiString GetItemCaption(AnsiString rootPath, AnsiString name);
	BEGIN_MESSAGE_MAP
	MESSAGE_HANDLER(WM_DROPFILES, TWMDropFiles, WMDropFiles)	
	END_MESSAGE_MAP(TForm);	
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
#endif
