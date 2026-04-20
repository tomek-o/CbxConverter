//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormSettings.h"
#include <FileCtrl.hpp>
#include <algorithm>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmSettings *frmSettings;
//---------------------------------------------------------------------------
__fastcall TfrmSettings::TfrmSettings(TComponent* Owner)
	: TForm(Owner)
{
	this->appSettings = NULL;
	cbTmpDirectoryType->Items->Clear();
	for (int i=0; i<Settings::_Directories::DirectoryTypeLimiter; i++)
	{
		cbTmpDirectoryType->Items->Add(Settings::_Directories::DirectoryTypeDescription(static_cast<enum Settings::_Directories::DirectoryType>(i)));
	}
	cbOutDirectoryType->Items->Clear();
	for (int i=0; i<Settings::_Directories::DirectoryTypeLimiter; i++)
	{
		cbOutDirectoryType->Items->Add(Settings::_Directories::DirectoryTypeDescription(static_cast<enum Settings::_Directories::DirectoryType>(i)));
	}
	for (int i=0; i<Settings::_Worker::THREAD_COUNT_MAX; i++)
	{
		cbWorkerThreadCount->Items->Add(i+1);
	}
	pcGeneral->ActivePage = tsGeneral;
}
//---------------------------------------------------------------------------
void __fastcall TfrmSettings::FormShow(TObject *Sender)
{
    assert(appSettings);
	tmpSettings = *appSettings;
	chbAlwaysOnTop->Checked = tmpSettings.frmMain.bAlwaysOnTop;

	chbLogToFile->Checked = tmpSettings.Logging.bLogToFile;
	cmbMaxUiLogLines->ItemIndex = -1;
	for (int i=0; i<cmbMaxUiLogLines->Items->Count; i++)
	{
		if ((unsigned int)StrToInt(cmbMaxUiLogLines->Items->Strings[i]) >= tmpSettings.Logging.iMaxUiLogLines)
		{
			cmbMaxUiLogLines->ItemIndex = i;
			break;
		}
	}
	if (cmbMaxUiLogLines->ItemIndex == -1)
	{
		cmbMaxUiLogLines->ItemHeight = cmbMaxUiLogLines->Items->Count - 1;
	}

	switch (tmpSettings.Worker.priority)
	{
		case IDLE_PRIORITY_CLASS:
			cbWorkerProcPriority->ItemIndex = 0;
			break;
		case NORMAL_PRIORITY_CLASS:
			cbWorkerProcPriority->ItemIndex = 2;
			break;
		case ABOVE_NORMAL_PRIORITY_CLASS:
			cbWorkerProcPriority->ItemIndex = 3;
			break;
		default:
			cbWorkerProcPriority->ItemIndex = 1;
			break;
	}

	cbWorkerThreadCount->ItemIndex = tmpSettings.Worker.threadCount - 1;

	chbPlaySoundWhenDone->Checked = tmpSettings.Worker.playSoundWhenDone;

	vledResizing->Strings->Clear();
	for (unsigned int i=0; i<sizeof(tmpSettings.Conversion.DefResize)/sizeof(tmpSettings.Conversion.DefResize[0]); i++)
	{
		if (tmpSettings.Conversion.DefResize[i].width == 0)
			continue;
		AnsiString tmp;
		tmp.sprintf("%d=%d",
			tmpSettings.Conversion.DefResize[i].width,
			tmpSettings.Conversion.DefResize[i].resize);
		vledResizing->Strings->Add(tmp);
	}
	edImExtraParameters->Text = tmpSettings.Conversion.sImExtraParams;
	edImExtraParametersBeforeResize->Text =	tmpSettings.Conversion.sImExtraParamsBeforeResize;
	edOutputExtension->Text = tmpSettings.Conversion.sOutputExtension;
	edConversionFilesToSkip->Text = tmpSettings.Conversion.filesToSkip;
	edUnpackPassword->Text = tmpSettings.Conversion.unpackPassword;
	chbCopyFileToOutputOnError->Checked = tmpSettings.Conversion.copyFileToOutputOnError;

	edGsLocation->Text = tmpSettings.PdfImport.sGsLocation;
	edGsParams->Text = tmpSettings.PdfImport.sGsParams;
	edGsFilePattern->Text = tmpSettings.PdfImport.sGsFilePattern;

	cbTmpDirectoryType->ItemIndex = tmpSettings.Directories.tmpDirectoryType;
	edTmpDirectory->Text = tmpSettings.Directories.customTmpDirectory;
	cbOutDirectoryType->ItemIndex = tmpSettings.Directories.outDirectoryType;
	edOutDirectory->Text = tmpSettings.Directories.customOutDirectory;

	chbUseSourceDirectoryForOutput->Checked = tmpSettings.Directories.useSourceDirectoryForOutput;
	chbRecreateSourceDirectoryForOutput->Checked = tmpSettings.Directories.recreateSourceDirectoryForOutput;
	chbUseNumericTmpFileDirectory->Checked = tmpSettings.Directories.useNumericTmpFileDirectory;

	UpdateDirectoriesPage();
}
//---------------------------------------------------------------------------
void __fastcall TfrmSettings::btnCancelClick(TObject *Sender)
{
	this->Close();	
}
//---------------------------------------------------------------------------
void __fastcall TfrmSettings::btnApplyClick(TObject *Sender)
{
	tmpSettings.Logging.bLogToFile = chbLogToFile->Checked;

	switch (cbWorkerProcPriority->ItemIndex)
	{
		case 0:
			tmpSettings.Worker.priority = IDLE_PRIORITY_CLASS;
			break;
		case 2:
			tmpSettings.Worker.priority = NORMAL_PRIORITY_CLASS;
			break;
		case 3:
			tmpSettings.Worker.priority = ABOVE_NORMAL_PRIORITY_CLASS;
			break;
		default:
			tmpSettings.Worker.priority = BELOW_NORMAL_PRIORITY_CLASS;
			break;
	}
	tmpSettings.Worker.playSoundWhenDone = chbPlaySoundWhenDone->Checked;

	tmpSettings.Worker.threadCount = cbWorkerThreadCount->ItemIndex + 1;

	memset(tmpSettings.Conversion.DefResize, 0, sizeof(tmpSettings.Conversion.DefResize));	
	for (int i=0; i<std::min<int>(vledResizing->Strings->Count,
		sizeof(tmpSettings.Conversion.DefResize)/sizeof(tmpSettings.Conversion.DefResize[0])); i++)
	{
		AnsiString key = vledResizing->Keys[i+1];
		AnsiString val = vledResizing->Values[key];
		tmpSettings.Conversion.DefResize[i].width = StrToIntDef(key, 0);
		tmpSettings.Conversion.DefResize[i].resize = StrToIntDef(val, 100);
	}
	tmpSettings.Conversion.sImExtraParams = edImExtraParameters->Text;
	tmpSettings.Conversion.sImExtraParamsBeforeResize = edImExtraParametersBeforeResize->Text;
	tmpSettings.Conversion.sOutputExtension = edOutputExtension->Text;
	tmpSettings.Conversion.filesToSkip = edConversionFilesToSkip->Text;
	tmpSettings.Conversion.unpackPassword = edUnpackPassword->Text;
	tmpSettings.Conversion.copyFileToOutputOnError = chbCopyFileToOutputOnError->Checked;

	tmpSettings.PdfImport.sGsLocation = edGsLocation->Text;
	tmpSettings.PdfImport.sGsParams = edGsParams->Text;
	tmpSettings.PdfImport.sGsFilePattern = edGsFilePattern->Text;

	tmpSettings.Directories.tmpDirectoryType = static_cast<Settings::_Directories::DirectoryType>(cbTmpDirectoryType->ItemIndex);
	tmpSettings.Directories.customTmpDirectory = edTmpDirectory->Text;
	tmpSettings.Directories.outDirectoryType = static_cast<Settings::_Directories::DirectoryType>(cbOutDirectoryType->ItemIndex);
	tmpSettings.Directories.customOutDirectory = edOutDirectory->Text;

	tmpSettings.Directories.useSourceDirectoryForOutput = chbUseSourceDirectoryForOutput->Checked;
	tmpSettings.Directories.recreateSourceDirectoryForOutput = chbRecreateSourceDirectoryForOutput->Checked;
	tmpSettings.Directories.useNumericTmpFileDirectory = chbUseNumericTmpFileDirectory->Checked;

	*appSettings = tmpSettings;
	this->Close();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::chbAlwaysOnTopClick(TObject *Sender)
{
	tmpSettings.frmMain.bAlwaysOnTop = chbAlwaysOnTop->Checked;	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::cmbMaxUiLogLinesChange(TObject *Sender)
{
	tmpSettings.Logging.iMaxUiLogLines = StrToInt(cmbMaxUiLogLines->Text);	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::btnSelectTmpDirectoryClick(TObject *Sender)
{
	TEdit* ed;
	if (Sender == btnSelectTmpDirectory)
		ed = edTmpDirectory;
	else if (Sender == btnSelectOutDirectory)
		ed = edOutDirectory;
	else
		ShowMessage("Unhandled sender!");

	AnsiString asDir;
	if (DirectoryExists(ed->Text))
		asDir = ed->Text;
	if (SelectDirectory((AnsiString)"Directory:", "", asDir))
		ed->Text = asDir;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::cbTmpDirectoryTypeChange(TObject *Sender)
{
	UpdateDirectoriesPage();
}
//---------------------------------------------------------------------------

void TfrmSettings::UpdateDirectoriesPage(void)
{
	if (cbTmpDirectoryType->ItemIndex == Settings::_Directories::DirectoryTypeDefault)
	{
		edTmpDirectory->Visible = false;
		btnSelectTmpDirectory->Visible = false;
	}
	else
	{
		edTmpDirectory->Visible = true;
		btnSelectTmpDirectory->Visible = true;
	}

	if (cbOutDirectoryType->ItemIndex == Settings::_Directories::DirectoryTypeDefault)
	{
		edOutDirectory->Visible = false;
		btnSelectOutDirectory->Visible = false;
	}
	else
	{
		edOutDirectory->Visible = true;
		btnSelectOutDirectory->Visible = true;
	}

	bool checked = chbUseSourceDirectoryForOutput->Checked;
	cbOutDirectoryType->Enabled = !checked;
	edOutDirectory->Enabled = !checked;
	lblOutDirectoryLocation->Enabled = !checked;
	btnSelectOutDirectory->Enabled = !checked;
	chbRecreateSourceDirectoryForOutput->Visible = !checked;
}

void __fastcall TfrmSettings::cbOutDirectoryTypeChange(TObject *Sender)
{
	UpdateDirectoriesPage();
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::btnGsLocationClick(TObject *Sender)
{
	openDialog->Filter = "Executable (*.exe)|*.exe|All files|*.*";
	openDialog->FilterIndex = 0;
	AnsiString file = edGsLocation->Text;
	if (FileExists(file))
	{
		AnsiString dir = ExtractFileDir(file);
		openDialog->InitialDir = dir;
		openDialog->FileName = file;
	}
	else
	{
		openDialog->FileName = "";
	}
	if (openDialog->Execute())
	{
        edGsLocation->Text = openDialog->FileName;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::FormKeyPress(TObject *Sender, char &Key)
{
	if (Key == VK_ESCAPE)
	{
		Close();
	}
}
//---------------------------------------------------------------------------


void __fastcall TfrmSettings::chbUseSourceDirectoryForOutputClick(
	  TObject *Sender)
{
	UpdateDirectoriesPage();
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::btnAddResizingEntryClick(TObject *Sender)
{
	vledResizing->Strings->Add("=");
	//vledResizing->SetFocus();
	//vledResizing->ScrollBy(0, 100);
}
//---------------------------------------------------------------------------

