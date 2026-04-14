//---------------------------------------------------------------------------

#ifndef FormSettingsH
#define FormSettingsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
#include "Settings.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <ValEdit.hpp>
#include <FileCtrl.hpp>
#include <Dialogs.hpp>

class TfrmSettings : public TForm
{
__published:	// IDE-managed Components
	TPanel *pnlBottom;
	TButton *btnCancel;
	TButton *btnApply;
	TPageControl *pcGeneral;
	TTabSheet *tsGeneral;
	TTabSheet *pcLogging;
	TCheckBox *chbAlwaysOnTop;
	TLabel *lblUiCapacity;
	TComboBox *cmbMaxUiLogLines;
	TCheckBox *chbLogToFile;
	TComboBox *cbWorkerProcPriority;
	TLabel *lblProcessPriority;
	TCheckBox *chbPlaySoundWhenDone;
	TValueListEditor *vledResizing;
	TLabel *Label1;
	TLabel *Label2;
	TEdit *edImExtraParameters;
	TLabel *Label3;
	TEdit *edOutputExtension;
	TTabSheet *pcDirectories;
	TLabel *lblTmpDirectoryType;
	TComboBox *cbTmpDirectoryType;
	TEdit *edTmpDirectory;
	TLabel *lblOutDirectoryLocation;
	TComboBox *cbOutDirectoryType;
	TEdit *edOutDirectory;
	TButton *btnSelectTmpDirectory;
	TButton *btnSelectOutDirectory;
	TLabel *lblImExtraAfterResize;
	TLabel *lblImExtraBeforeResize;
	TEdit *edImExtraParametersBeforeResize;
	TTabSheet *pcPdfImport;
	TLabel *lblPdfImportInfo;
	TEdit *edGsLocation;
	TButton *btnGsLocation;
	TLabel *lblGsLocation;
	TOpenDialog *openDialog;
	TLabel *lblGsParameters;
	TEdit *edGsParams;
	TLabel *lblFilePattern;
	TEdit *edGsFilePattern;
	TCheckBox *chbUseSourceDirectoryForOutput;
	TCheckBox *chbUseNumericTmpFileDirectory;
	TCheckBox *chbRecreateSourceDirectoryForOutput;
	TLabel *lblWorkerThreadCount;
	TComboBox *cbWorkerThreadCount;
	TEdit *edConversionFilesToSkip;
	TLabel *lblFilesToSkip;
	TLabel *lblUnpackPassword;
	TEdit *edUnpackPassword;
	TCheckBox *chbCopyFileToOutputOnError;
	TButton *btnAddResizingEntry;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btnCancelClick(TObject *Sender);
	void __fastcall btnApplyClick(TObject *Sender);
	void __fastcall chbAlwaysOnTopClick(TObject *Sender);
	void __fastcall cmbMaxUiLogLinesChange(TObject *Sender);
	void __fastcall btnSelectTmpDirectoryClick(TObject *Sender);
	void __fastcall cbTmpDirectoryTypeChange(TObject *Sender);
	void __fastcall cbOutDirectoryTypeChange(TObject *Sender);
	void __fastcall btnGsLocationClick(TObject *Sender);
	void __fastcall FormKeyPress(TObject *Sender, char &Key);
	void __fastcall chbUseSourceDirectoryForOutputClick(TObject *Sender);
	void __fastcall btnAddResizingEntryClick(TObject *Sender);
private:	// User declarations
	void UpdateDirectoriesPage(void);
public:		// User declarations
	__fastcall TfrmSettings(TComponent* Owner);
	Settings *appSettings;
	Settings tmpSettings;
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmSettings *frmSettings;
//---------------------------------------------------------------------------
#endif
