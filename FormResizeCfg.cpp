//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormResizeCfg.h"
#include "ResizeCfg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmResizeCfg *frmResizeCfg;
//---------------------------------------------------------------------------
__fastcall TfrmResizeCfg::TfrmResizeCfg(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void TfrmResizeCfg::SetCfg(ResizeCfg *cfg)
{
	this->cfg = cfg;
	cbResizeMode->ItemIndex = cfg->mode;
	cbConditionalResizeMode->ItemIndex = cfg->modeOnOversize;
	cbResizePct->Text = cfg->resizePct;
	edResizeThreshold->Text = cfg->resizeThreshold;
	edResizeTarget->Text = cfg->resizeTarget;
	cbResizeModeChange(NULL);
}

void __fastcall TfrmResizeCfg::btnApplyClick(TObject *Sender)
{
	ResizeCfg tmp;
	try
	{
		tmp.mode = static_cast<ResizeCfg::Mode>(cbResizeMode->ItemIndex);
		tmp.modeOnOversize = static_cast<ResizeCfg::ModeOnOversizeSub>(cbConditionalResizeMode->ItemIndex);
		tmp.resizePct = StrToInt(cbResizePct->Text);
		tmp.resizeThreshold = StrToInt(edResizeThreshold->Text);
		tmp.resizeTarget = StrToInt(edResizeTarget->Text);
		*cfg = tmp;
		ModalResult = mrOk;
	}
	catch (...)
	{
		ShowMessage("Invalid entry!");
		ModalResult = mrNone;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmResizeCfg::cbResizeModeChange(TObject *Sender)
{
	lblResizeTo->Visible = false;
	lblResizeToPct->Visible = false;
	cbResizePct->Visible = false;

	cbConditionalResizeMode->Visible = false;
	lblResizeIf->Visible = false;
	lblThan->Visible = false;
	edResizeThreshold->Visible = false;
	lblResizeIfPxTo->Visible = false;
	edResizeTarget->Visible = false;
	lblResizeIfPx->Visible = false;

	if (cbResizeMode->ItemIndex == 0)
	{
   		lblResizeTo->Visible = true;
		lblResizeToPct->Visible = true;
		cbResizePct->Visible = true;
	}
	else
	{
		cbConditionalResizeMode->Visible = true;
		lblThan->Visible = true;
		lblResizeIf->Visible = true;
		edResizeThreshold->Visible = true;
		lblResizeIfPxTo->Visible = true;
		edResizeTarget->Visible = true;
		lblResizeIfPx->Visible = true;
    }	
}
//---------------------------------------------------------------------------

void __fastcall TfrmResizeCfg::FormKeyPress(TObject *Sender, char &Key)
{
	if (Key == VK_ESCAPE)
	{
		Close();
	}	
}
//---------------------------------------------------------------------------

