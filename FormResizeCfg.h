//---------------------------------------------------------------------------

#ifndef FormResizeCfgH
#define FormResizeCfgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

struct ResizeCfg;

class TfrmResizeCfg : public TForm
{
__published:	// IDE-managed Components
	TPanel *pnlBottom;
	TButton *btnCancel;
	TButton *btnApply;
	TComboBox *cbResizePct;
	TLabel *lblResizeTo;
	TLabel *lblResizeToPct;
	TEdit *edResizeThreshold;
	TComboBox *cbResizeMode;
	TLabel *lblResizeMode;
	TLabel *lblResizeIf;
	TLabel *lblResizeIfPxTo;
	TEdit *edResizeTarget;
	TLabel *lblResizeIfPx;
	TComboBox *cbConditionalResizeMode;
	TLabel *lblThan;
	void __fastcall btnApplyClick(TObject *Sender);
	void __fastcall cbResizeModeChange(TObject *Sender);
	void __fastcall FormKeyPress(TObject *Sender, char &Key);
private:	// User declarations
	ResizeCfg *cfg;
public:		// User declarations
	__fastcall TfrmResizeCfg(TComponent* Owner);
	void SetCfg(ResizeCfg *cfg);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmResizeCfg *frmResizeCfg;
//---------------------------------------------------------------------------
#endif
