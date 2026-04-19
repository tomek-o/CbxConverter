//---------------------------------------------------------------------------

#ifndef FormRenameH
#define FormRenameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------

struct SourceFile;

class TfrmRename : public TForm
{
__published:	// IDE-managed Components
	TPanel *pnlBottom;
	TListView *lvFiles;
	TButton *btnRename;
	TComboBox *cbRenameType;
	TLabel *lblPattern;
	TEdit *edPattern;
	TLabel *lblOffset;
	TEdit *edOffset;
	TPopupMenu *popupList;
	TMenuItem *miDeleteSelectedFiles;
	TMenuItem *miOpenFile;
	void __fastcall lvFilesData(TObject *Sender, TListItem *Item);
	void __fastcall cbRenameTypeChange(TObject *Sender);
	void __fastcall edOffsetChange(TObject *Sender);
	void __fastcall edPatternChange(TObject *Sender);
	void __fastcall btnRenameClick(TObject *Sender);
	void __fastcall miDeleteSelectedFilesClick(TObject *Sender);
	void __fastcall miOpenFileClick(TObject *Sender);
private:	// User declarations
	enum RenameType {
		RenameExtractingNumberFromName = 0,
		RenameStartingFromNumber
	} renameType;
	int offset;
	AnsiString pattern;
	SourceFile *sourceFile;
	AnsiString GetNewName(AnsiString name, int id);
public:		// User declarations
	__fastcall TfrmRename(TComponent* Owner);
#pragma warn -8022
	void ShowModal(SourceFile *sourceFile);
#pragma warn .8022
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmRename *frmRename;
//---------------------------------------------------------------------------
#endif
