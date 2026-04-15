//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormMain.h"
#include "ThreadWorker.h"
#include "FormAbout.h"
#include "FormSettings.h"
#include "FormResizeCfg.h"
#include "FormRename.h"
#include "Utils.h"
#include "LogUnit.h"
#include "Log.h"
#include "JobQueue.h"
#include "Mutex.h"
#include "ScopedLock.h"

#include <sys\stat.h>
#include <algorithm>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMain *frmMain;
//---------------------------------------------------------------------------

namespace {

Mutex mutex;
unsigned int currentDirectoryId = 0;

AnsiString tmpPath;

AnsiString GetTmpDir(AnsiString fileName)
{
	AnsiString dir;
	if (appSettings.Directories.useNumericTmpFileDirectory == false)
	{
		dir = tmpPath + ExtractFileName(fileName) + "\\";
	}
	else
	{
		ScopedLock<Mutex> lock(mutex);
		AnsiString tmp;
		tmp.sprintf("%04u", currentDirectoryId);
		currentDirectoryId++;
		dir = tmpPath + tmp + "\\";
	}
	return dir;
}

}

__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner)
{
	DoubleBuffered = true;
	lvInput->DoubleBuffered = true;
	// inform OS that we accepting dropping files
	DragAcceptFiles(Handle, True);

	jobQueue = new JobQueue();	
}

__fastcall TfrmMain::~TfrmMain(void)
{
	if (jobQueue)
	{
		delete jobQueue;
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCreate(TObject *Sender)
{
	AnsiString asConfigFile = ChangeFileExt( Application->ExeName, ".ini" );
	appSettings.Read(asConfigFile);
	if (this->BorderStyle != bsSingle)
	{
		this->Width = appSettings.frmMain.iWidth;
		this->Height = appSettings.frmMain.iHeight;
	}
	this->Top = appSettings.frmMain.iPosY;
	this->Left = appSettings.frmMain.iPosX;
	if (appSettings.frmMain.bWindowMaximized)
		this->WindowState = wsMaximized;
}

void TfrmMain::ApplySettings(void)
{
	if (appSettings.frmMain.bAlwaysOnTop)
		this->FormStyle = fsStayOnTop;
	else
		this->FormStyle = fsNormal;
	if (appSettings.Logging.bLogToFile)
		CLog::Instance()->SetFile(ChangeFileExt(Application->ExeName, ".log").c_str());
	else
		CLog::Instance()->SetFile("");
	frmLog->SetLogLinesLimit(appSettings.Logging.iMaxUiLogLines);

	switch (appSettings.Directories.tmpDirectoryType)
	{
	case Settings::_Directories::DirectoryTypeDefault:
		tmpPath = ExtractFilePath(Application->ExeName) + "tmp\\";
		break;
	case Settings::_Directories::DirectoryTypeCustom:
		tmpPath = appSettings.Directories.customTmpDirectory;
		break;
	default:
		assert(!"Unhandled directory type!");
		break;
	}
	if (tmpPath[tmpPath.Length()] != '\\')
		tmpPath += "\\";	
	Job::SetTmpPath(tmpPath);

	AnsiString outPath;
	switch (appSettings.Directories.outDirectoryType)
	{
	case Settings::_Directories::DirectoryTypeDefault:
		outPath = ExtractFilePath(Application->ExeName) + "out\\";
		break;
	case Settings::_Directories::DirectoryTypeCustom:
		outPath = appSettings.Directories.customOutDirectory;
		break;
	default:
		assert(!"Unhandled directory type!");
		break;
	}
	Job::SetOutPath(outPath);
	Job::SetUseSourceDirectoryForOutput(appSettings.Directories.useSourceDirectoryForOutput);
	Job::SetRecreateSourceDirectoryForOutput(appSettings.Directories.recreateSourceDirectoryForOutput);

	Job::SetGsPath(appSettings.PdfImport.sGsLocation);
	Job::SetGsParams(appSettings.PdfImport.sGsParams);
	Job::SetGsFilePattern(appSettings.PdfImport.sGsFilePattern);
}

//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	AnsiString asConfigFile = ChangeFileExt( Application->ExeName, ".ini" );
	appSettings.frmMain.bWindowMaximized = (this->WindowState == wsMaximized);
	if (!appSettings.frmMain.bWindowMaximized)
	{
		// these values are meaningless is wnd is maximized
		appSettings.frmMain.iWidth = this->Width;
		appSettings.frmMain.iHeight = this->Height;
		appSettings.frmMain.iPosY = this->Top;
		appSettings.frmMain.iPosX = this->Left;
	}
	appSettings.Write(asConfigFile);

	jobQueue->AbortAll();

	CanClose = true;	
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actShowAboutExecute(TObject *Sender)
{
	frmAbout->ShowModal();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actShowSettingsExecute(TObject *Sender)
{
	frmSettings->appSettings = &appSettings;
	frmSettings->ShowModal();
	ApplySettings();
	AnsiString asConfigFile = ChangeFileExt( Application->ExeName, ".ini" );
	appSettings.Write(asConfigFile);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormShow(TObject *Sender)
{
    static bool once = false;
    if (!once)
    {
		once = true;
		frmLog->SetLogLinesLimit(appSettings.Logging.iMaxUiLogLines);
		CLog::Instance()->SetLevel(E_LOG_TRACE);
		CLog::Instance()->callbackLog = frmLog->OnLog;

		if (appSettings.Directories.tmpDirectoryType == Settings::_Directories::DirectoryTypeDefault)
		{
			AnsiString dir = ExtractFileDir(Application->ExeName);
			if (dir.Length() > 25)
			{
				MessageBox(this->Handle, "To avoid possible problems with maximum path length CbxConverter should be placed near the drive root.",
					this->Caption.c_str(), MB_ICONEXCLAMATION);
			}
		}
		else
		{
			AnsiString dir = ExtractFileDir(Application->ExeName);
			if (dir.Length() > 200)
			{
				MessageBox(this->Handle, "To avoid possible problems with maximum path length CbxConverter should be placed near the drive root.",
					this->Caption.c_str(), MB_ICONEXCLAMATION);
			}
			if (appSettings.Directories.customTmpDirectory.Length() > 30)
			{
				MessageBox(this->Handle, "To avoid possible problems with maximum path avoid using long paths for custom tmp directory.",
					this->Caption.c_str(), MB_ICONEXCLAMATION);
			}
		}
	}
	LOG("Application started");
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormDestroy(TObject *Sender)
{
	CLog::Instance()->Destroy();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actShowLogExecute(TObject *Sender)
{
	if (!frmLog->Visible)
		frmLog->Show();
	frmLog->BringToFront();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miAddSourceFileClick(TObject *Sender)
{
	dlgOpen->Options.Clear();
	dlgOpen->Options << ofAllowMultiSelect << ofFileMustExist;
	dlgOpen->Filter =
		"Comic book files (*.cbz, *.cbr, *.cb7, *.cbt, *.pdf)|*.cbz;*.cbr;*.cb7;*.cbt;*.pdf|"
		"All files|*.*";
	dlgOpen->FilterIndex = 1;
	if (dlgOpen->Execute())
	{
		for (int i=0; i<dlgOpen->Files->Count; i++)
		{
			AddSourceFile(dlgOpen->Files->Strings[i], "");
		}
	}
}
//---------------------------------------------------------------------------


// fires an event when a file, or files are dropped onto the application.
void __fastcall TfrmMain::WMDropFiles(TWMDropFiles &message)
{
    AnsiString FileName;
    char buff[MAX_PATH];

	int Count = DragQueryFile((HDROP)message.Drop, 0xFFFFFFFF, NULL, MAX_PATH);

	// index through the files and query the OS for each file name...
	// supporting dragging multiple files and folders (scanned recursively)
	for (int index = 0; index < Count; ++index)
	{
		// the following code gets the FileName of the dropped file. it
		// looks cryptic but that's only because it is. Hey, Why do you think
		// Delphi and C++Builder are so popular anyway? Look up DragQueryFile
		// the Win32.hlp Windows API help file.
		DragQueryFile((HDROP)message.Drop, index, buff, sizeof(buff));
		FileName = buff;
		LOG("Dropped into application: %s", FileName.c_str());
		if (DirectoryExists(FileName))
		{
			AnsiString rootPath = "";
			const char* ptr = strrchr(FileName.c_str(), '\\');
			if (ptr)
			{
            	rootPath = FileName.SubString(1, ptr - FileName.c_str());
			}
			AddSourceDir(FileName + "\\*.*", rootPath);
		}
		else
		{
			AddSourceFile(FileName, "");
		}
	}

    // tell the OS that you're finished...
    DragFinish((HDROP) message.Drop);
}

void TfrmMain::AddSourceFile(AnsiString name, AnsiString rootPath)
{
	struct stati64 statbuf;
	_stati64(name.c_str(), &statbuf);

	if (ExtractFileExt(name) == ".pdf")
	{
		if (appSettings.PdfImport.sGsLocation == "")
		{
			LOG("Install GhostScript and set gswin32c.exe path in configuration");
			return;
		}
		if (FileExists(appSettings.PdfImport.sGsLocation) == false)
		{
			LOG("gswin32c.exe location is not valid, see settings");
			return;
        }
    }
	SourceFile s;
	sourceFilesList.push_back(s);
	SourceFile *source = &(*(sourceFilesList.rbegin()));
	sourceFiles.push_back(source);
	source->name = name;
	source->rootPath = rootPath;
	source->tmpDir = GetTmpDir(name);
	source->size = statbuf.st_size;
	UpdateSourceView();
#if 1
	Job job(source);
	if (ExtractFileExt(name) == ".pdf")
	{
		job.type = Job::RENDER_PDF;
	}
	else
	{
		job.type = Job::UNPACK;
	}
	job.fileName = name;
	source->jobCount++;
	jobQueue->Add(job);
#endif
}

void TfrmMain::AddSourceDir(AnsiString path, AnsiString rootPath)
{
    int iAttributes = faAnyFile;
	Sysutils::TSearchRec sr;
	LOG("Add source directory: %s, root path %s", path.c_str(), rootPath.c_str());
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
                        AddSourceDir(drillPath, rootPath);
					}
				}
			}
			else
			{
				AnsiString name = ExtractFilePath(path) + sr.Name;
				AddSourceFile(name, rootPath);
			}
		} while (FindNext(sr) == 0);
		FindClose(sr);
	}
}

void TfrmMain::UpdateSourceView(void)
{
	lvInput->Items->Count = sourceFiles.size();
	lvInput->Invalidate();
}

void __fastcall TfrmMain::lvInputData(TObject *Sender, TListItem *Item)
{
	int id = Item->Index;
	SourceFile *source = sourceFiles[id];

	Item->Caption = GetItemCaption(source->rootPath, source->name);

	Item->SubItems->Add(source->size/1024);
	if (source->unpacked)
	{
		Item->SubItems->Add(source->imgCount);
		if (source->imgCount)
		{
			Item->SubItems->Add(source->size/source->imgCount / 1024);
		}
		else
		{
        	Item->SubItems->Add("-");
		}
	}
	else
	{
    	Item->SubItems->Add("?");
    	Item->SubItems->Add("?");
	}
	if (source->widthValid)
	{
		Item->SubItems->Add(source->asWidth);
	}
	else
	{
    	Item->SubItems->Add("not checked");
	}
	AnsiString text;
	ResizeCfg &resizeCfg = source->resizeCfg;
	if (resizeCfg.mode == ResizeCfg::ModeRegular)
	{
		text.sprintf("%d%%", resizeCfg.resizePct);
	}
	else if (resizeCfg.mode == ResizeCfg::ModeOnOversize)
	{
		text.sprintf("if > %d", resizeCfg.resizeThreshold);
	}
	else
	{
		assert(!"Unhandled resize mode");
    }
	Item->SubItems->Add(text);
	
	Item->SubItems->Add(source->stateName());

	if (source->outSizeValid)
	{
		Item->SubItems->Add(source->outSize/1024);
		if (source->size != 0)
		{
			AnsiString pct;
			pct.sprintf("%.1f%%", ((double)source->outSize) * 100.0 / source->size);
			Item->SubItems->Add(pct);
		}
		else
		{
            Item->SubItems->Add("---");
        }
	}
	else
	{
		Item->SubItems->Add("---");
		Item->SubItems->Add("---");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::btnStartConversionClick(TObject *Sender)
{
	for (unsigned int i=0; i<sourceFiles.size(); i++)
	{
		SourceFile *source = sourceFiles[i];
		if (source->state != SourceFile::DONE)
		{
			Job job(source);
			job.type = Job::CONVERT;
			job.fileName = source->name;
			job.resizeCfg = source->resizeCfg;
			job.sImExtraParamsBeforeResize = appSettings.Conversion.sImExtraParamsBeforeResize;
			job.sImExtraParams = appSettings.Conversion.sImExtraParams;
			job.sOutputExtension = appSettings.Conversion.sOutputExtension;
			source->jobCount++;
			jobQueue->Add(job);
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::tmrRefreshStateViewTimer(TObject *Sender)
{
	// ugly: apply default resizing
	std::vector<SourceFile*>::iterator iter = sourceFiles.begin();
	while (iter != sourceFiles.end())
	{
		if ((*iter)->state == SourceFile::IDLE &&
			(*iter)->unpacked &&
			(*iter)->widthValid &&
			(*iter)->defaultResizeApplied == false)
		{
		    (*iter)->resizeCfg.resizePct = GetDefaultResize((*iter)->mostFrequentWidth);
			(*iter)->defaultResizeApplied = true;
		}
		++iter;
	}

	lvInput->Invalidate();
}
//---------------------------------------------------------------------------

int TfrmMain::GetDefaultResize(int width)
{
	if (width > 0)
	{
		for (unsigned int i=0; i<sizeof(appSettings.Conversion.DefResize)/sizeof(appSettings.Conversion.DefResize[0]); i++)
		{
			if (appSettings.Conversion.DefResize[i].width == width)
			{
				return appSettings.Conversion.DefResize[i].resize;
			}
		}
	}
	return 100;
}

void __fastcall TfrmMain::miResizeClick(TObject *Sender)
{
	if (!lvInput->Selected)
		return;
	int id = lvInput->Selected->Index;
	SourceFile *selectedFile = sourceFiles[id];
	ResizeCfg &cfg = selectedFile->resizeCfg;

	frmResizeCfg->SetCfg(&cfg);
	frmResizeCfg->ShowModal();
	int mr = frmResizeCfg->ModalResult;
	if (mr != mrOk)
	{
		return;
    }
	for (int i=0; i<lvInput->Items->Count; i++)
	{
		TListItem *item = lvInput->Items->Item[i];
		if (item->Selected)
		{
			SourceFile *source = sourceFiles[i];
			source->resizeCfg = cfg;
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miClearConvertedClick(TObject *Sender)
{
	if (!jobQueue->isEmpty())
	{
		ShowMessage("Cannot modify list while converting");
		return;
	}
	tmrRefreshStateView->Enabled = false;

	std::vector<SourceFile*>::iterator iter = sourceFiles.begin();
	while (iter != sourceFiles.end())
    {
        if ((*iter)->state == SourceFile::DONE)
		{
			//SourceFile &f = **iter;
			AnsiString name = (*iter)->name;
			std::list<SourceFile>::iterator itlist;
			for (itlist = sourceFilesList.begin(); itlist != sourceFilesList.end(); ++itlist)
			{
				if (itlist->name == name)
				{
					sourceFilesList.erase(itlist);
					break;
				}
			}
			//sourceFilesList.remove(f);
			iter = sourceFiles.erase(iter);
        }
        else
        {
           ++iter;
		}
	}
	lvInput->Items->Count = sourceFiles.size();
	lvInput->Invalidate();
	tmrRefreshStateView->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miRemoveFileClick(TObject *Sender)
{
	if (!jobQueue->isEmpty())
	{
		ShowMessage("Cannot modify list while converting");
		return;
	}
	tmrRefreshStateView->Enabled = false;

	std::vector<SourceFile*>::iterator iter = sourceFiles.begin();
	std::vector<bool> selected;
	for (int i=0; i<lvInput->Items->Count; i++)
	{
		selected.push_back(lvInput->Items->Item[i]->Selected);
	}

	int id = 0;
	while (iter != sourceFiles.end())
	{
		bool erased = false;
		if (selected[id])
		{
			if ((*iter)->state == SourceFile::DONE
				|| (*iter)->state == SourceFile::IDLE
				|| (*iter)->state == SourceFile::SOURCE_ERROR				
				)
			{
				//SourceFile &f = **iter;
				AnsiString name = (*iter)->name;

				if ((*iter)->unpacked)
				{
					AnsiString dir = (*iter)->tmpDir;
					// remove existing directory if exists
					if (DirectoryExists(dir))
					{
						AnsiString d = dir.SubString(1, dir.Length() - 1);
						DeleteDirectory(NULL, d, true);
					}
				}

				std::list<SourceFile>::iterator itlist;
				for (itlist = sourceFilesList.begin(); itlist != sourceFilesList.end(); ++itlist)
				{
					if (itlist->name == name)
					{
						sourceFilesList.erase(itlist);
						break;
					}
				}
				//sourceFilesList.remove(f);
				iter = sourceFiles.erase(iter);
				erased = true;
			}
		}
		if (!erased)
		{
		   ++iter;
		}
		id++;
	}
	lvInput->Items->Count = sourceFiles.size();
	lvInput->Invalidate();

	tmrRefreshStateView->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::tmrStartupTimer(TObject *Sender)
{
	tmrStartup->Enabled = false;
	ApplySettings();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miExitClick(TObject *Sender)
{
	this->Close();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miDeleteSourceFilesClick(TObject *Sender)
{
	int count = lvInput->SelCount;
	if (count == 0)
	{
		MessageBox(this->Handle, "No files selected", this->Caption.c_str(), MB_OK);
		return;
    }
	AnsiString msg;
	msg.sprintf("Delete %d source file(s)?", count);
	if (MessageBox(this->Handle, msg.c_str(),
		this->Caption.c_str(), MB_YESNO | MB_DEFBUTTON2 | MB_ICONEXCLAMATION) != IDYES)
	{
		return;
	}
	int delCount = 0;
	for (int i=0; i<lvInput->Items->Count; i++)
	{
		if (lvInput->Items->Item[i]->Selected)
		{
			SourceFile *source = sourceFiles[i];
			if (source->state == SourceFile::UNPACKING)
			{
				ShowMessage("Cannot delete file that is unpacking right now, skipping");
				continue;
			}
			FileSetAttr(source->name, 0);	// remove read-only attribute
			if (DeleteFile(source->name) == false)
			{
				LOG("Failed to delete source file: %s", source->name.c_str());
			}
			else
			{
				LOG("Deleted %s", source->name.c_str());
				delCount++;
            }
        }
	}
	msg.sprintf("Deleted %d file(s)", delCount);
	MessageBox(this->Handle, msg.c_str(), this->Caption.c_str(), MB_OK | MB_ICONINFORMATION);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miOpenTmpDirectoryClick(TObject *Sender)
{
	ShellExecute(NULL, "open", Job::GetTmpPath().c_str(), NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miOpenOutputDirectoryClick(TObject *Sender)
{
	ShellExecute(NULL, "open", Job::GetOutPath().c_str(), NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miAddRecursivelyFilesFromDirectoryClick(
      TObject *Sender)
{
	AnsiString asDir;
	if (SelectDirectory((AnsiString)"Directory:", "", asDir))
	{
		if (DirectoryExists(asDir))
		{
			AnsiString rootPath = "";
			const char* ptr = strrchr(asDir.c_str(), '\\');
			if (ptr)
			{
				rootPath = asDir.SubString(1, ptr - asDir.c_str());
			}
			AddSourceDir(asDir + "\\*.*", rootPath);
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::tmrJobQueuePollTimer(TObject *Sender)
{
	if (jobQueue)
	{
		jobQueue->Poll();
		static bool allIdle = true;
		bool idle = jobQueue->isEmpty();
		if (!allIdle && idle)
		{
			if (appSettings.Worker.playSoundWhenDone)
			{
				PlaySound("SystemAsterisk", NULL, SND_ASYNC);
			}
		}
		allIdle = idle;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miDeleteOutputFilesClick(TObject *Sender)
{
	int count = lvInput->SelCount;
	if (count == 0)
	{
		MessageBox(this->Handle, "No files selected", this->Caption.c_str(), MB_OK);
		return;
    }
	AnsiString msg;
	msg.sprintf("Delete %d output file(s)?", count);
	if (MessageBox(this->Handle, msg.c_str(),
		this->Caption.c_str(), MB_YESNO | MB_DEFBUTTON2 | MB_ICONEXCLAMATION) != IDYES)
	{
		return;
	}
	int delCount = 0;
	for (int i=0; i<lvInput->Items->Count; i++)
	{
		if (lvInput->Items->Item[i]->Selected)
		{
			SourceFile *source = sourceFiles[i];
			FileSetAttr(source->outFile, 0);	// remove read-only attribute
			if (DeleteFile(source->outFile) == false)
			{
				LOG("Failed to delete output file: %s", source->outFile.c_str());
			}
			else
			{
				LOG("Deleted %s", source->outFile.c_str());
				delCount++;
            }
        }
	}
	msg.sprintf("Deleted %d file(s)", delCount);
	MessageBox(this->Handle, msg.c_str(), this->Caption.c_str(), MB_OK | MB_ICONINFORMATION);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::lvInputColumnClick(TObject *Sender,
      TListColumn *Column)
{
	if (Column->Index == 5 || Column->Index == 6)
		return;

	if (!jobQueue->isEmpty())
	{
		ShowMessage("Cannot modify list while converting");
		return;
	}

	bool ascending = true;
	if (Column->ImageIndex == 0)
	{
		ascending = false;
	}

	for (int i=0; i<lvInput->Columns->Count; i++)
	{
    	lvInput->Column[i]->ImageIndex = -1;
	}

	if (ascending)
	{
		Column->ImageIndex = 0;
	}
	else
	{
		Column->ImageIndex = 1;
	}

	enum SortType sortType;
	switch (Column->Index)
	{
	case 0:
		sortType = SortByFileName;
		break;
	case 1:
		sortType = SortBySize;
		break;
	case 2:
		sortType = SortByImageCnt;
		break;
	case 3:
		sortType = SortByImageSize;
		break;
	case 4:
		sortType = SortByImageWidth;
		break;
	case 7:
		sortType = SortByOutSize;
		break;
	case 8:
		sortType = SortByOutSizeRatio;
		break;
	default:
		assert(!"Unhandled sort column!");
		return;
	}

	sort(sortType, ascending);

	lvInput->Invalidate();
	
	lvInput->ClearSelection();
}
//---------------------------------------------------------------------------

namespace
{

bool compareFileNameAsc(const SourceFile& e1, const SourceFile& e2)
{
	return UpperCase(TfrmMain::GetItemCaption(e1.rootPath, e1.name)).AnsiCompare(UpperCase(TfrmMain::GetItemCaption(e2.rootPath, e2.name))) > 0;
}

bool compareFileNameDesc(const SourceFile& e1, const SourceFile& e2)
{
	return UpperCase(TfrmMain::GetItemCaption(e1.rootPath, e1.name)).AnsiCompare(UpperCase(TfrmMain::GetItemCaption(e2.rootPath, e2.name))) < 0;
}

bool compareSizeAsc(const SourceFile& e1, const SourceFile& e2)
{
	return e1.size > e2.size;
}

bool compareSizeDesc(const SourceFile& e1, const SourceFile& e2)
{
	return e1.size < e2.size;
}

bool compareImageCntAsc(const SourceFile& e1, const SourceFile& e2)
{
	return e1.fileDescs.size() > e2.fileDescs.size();
}

bool compareImageCntDesc(const SourceFile& e1, const SourceFile& e2)
{
	return e1.fileDescs.size() < e2.fileDescs.size();
}

bool compareImageSizeAsc(const SourceFile& e1, const SourceFile& e2)
{
	int64_t size1 = 0;
	if (e1.imgCount > 0)
		size1 = e1.size/e1.imgCount;
	int64_t size2 = 0;
	if (e2.imgCount > 0)
		size2 = e2.size/e2.imgCount;
	return size1 > size2;
}

bool compareImageSizeDesc(const SourceFile& e1, const SourceFile& e2)
{
	int64_t size1 = 0;
	if (e1.imgCount > 0)
		size1 = e1.size/e1.imgCount;
	int64_t size2 = 0;
	if (e2.imgCount > 0)
		size2 = e2.size/e2.imgCount;
	return size1 < size2;
}

bool compareImageWidthAsc(const SourceFile& e1, const SourceFile& e2)
{
	return e1.mostFrequentWidth > e2.mostFrequentWidth;
}

bool compareImageWidthDesc(const SourceFile& e1, const SourceFile& e2)
{
	return e1.mostFrequentWidth < e2.mostFrequentWidth;
}

bool compareOutSizeAsc(const SourceFile& e1, const SourceFile& e2)
{
	return e1.outSize > e2.outSize;
}

bool compareOutSizeDesc(const SourceFile& e1, const SourceFile& e2)
{
	return e1.outSize < e2.outSize;
}

bool compareOutSizeRatioAsc(const SourceFile& e1, const SourceFile& e2)
{
	float ratio1 = 0;
	if (e1.outSize != 0)
		ratio1 = static_cast<float>(e1.outSize) / e1.size;
	float ratio2 = 0;
	if (e2.outSize != 0)
		ratio2 = static_cast<float>(e2.outSize) / e2.size;
	return ratio1 > ratio2;
}

bool compareOutSizeRatioDesc(const SourceFile& e1, const SourceFile& e2)
{
	float ratio1 = 0;
	if (e1.outSize != 0)
		ratio1 = static_cast<float>(e1.outSize) / e1.size;
	float ratio2 = 0;
	if (e2.outSize != 0)
		ratio2 = static_cast<float>(e2.outSize) / e2.size;
	return ratio1 < ratio2;
}

}

int TfrmMain::sort(enum SortType type, bool ascending)
{
#pragma warn -8091	// incorrectly issued by BDS2006

	switch(type)
	{
	case SortByFileName:
		if (ascending)
			std::stable_sort(sourceFilesList.begin(), sourceFilesList.end(), compareFileNameAsc);
		else
			std::stable_sort(sourceFilesList.begin(), sourceFilesList.end(), compareFileNameDesc);
		break;
	case SortBySize:
		if (ascending)
			std::stable_sort(sourceFilesList.begin(), sourceFilesList.end(), compareSizeAsc);
		else
			std::stable_sort(sourceFilesList.begin(), sourceFilesList.end(), compareSizeDesc);
		break;
	case SortByImageCnt:
		if (ascending)
			std::stable_sort(sourceFilesList.begin(), sourceFilesList.end(), compareImageCntAsc);
		else
			std::stable_sort(sourceFilesList.begin(), sourceFilesList.end(), compareImageCntDesc);
		break;
	case SortByImageSize:
		if (ascending)
			std::stable_sort(sourceFilesList.begin(), sourceFilesList.end(), compareImageSizeAsc);
		else
			std::stable_sort(sourceFilesList.begin(), sourceFilesList.end(), compareImageSizeDesc);
		break;
	case SortByImageWidth:
		if (ascending)
			std::stable_sort(sourceFilesList.begin(), sourceFilesList.end(), compareImageWidthAsc);
		else
			std::stable_sort(sourceFilesList.begin(), sourceFilesList.end(), compareImageWidthDesc);
		break;
	case SortByOutSize:
		if (ascending)
			std::stable_sort(sourceFilesList.begin(), sourceFilesList.end(), compareOutSizeAsc);
		else
			std::stable_sort(sourceFilesList.begin(), sourceFilesList.end(), compareOutSizeDesc);
		break;
	case SortByOutSizeRatio:
		if (ascending)
			std::stable_sort(sourceFilesList.begin(), sourceFilesList.end(), compareOutSizeRatioAsc);
		else
			std::stable_sort(sourceFilesList.begin(), sourceFilesList.end(), compareOutSizeRatioDesc);
		break;

	default:
		return -1;
	}

	sourceFiles.clear();
	for (std::list<SourceFile>::iterator iter = sourceFilesList.begin(); iter!=sourceFilesList.end(); ++iter)
	{
    	sourceFiles.push_back(&(*iter));
	}
	return 0;
}

AnsiString TfrmMain::GetItemCaption(AnsiString rootPath, AnsiString name)
{
	AnsiString itemCaption = ExtractFileName(name);
	if (rootPath != "")
	{
		AnsiString fileDir = ExtractFileDir(name);
		AnsiString rootDir = rootPath;
		AnsiString delta = fileDir.SubString(rootDir.Length()+1, fileDir.Length()-rootDir.Length());
		if (delta[1] == '\\')
			delta = delta.SubString(2, delta.Length()-1);
		if (delta != "")
		{
			itemCaption = delta + "\\" + itemCaption;
		}
	}

	return itemCaption;
}

void __fastcall TfrmMain::miSelectAllClick(TObject *Sender)
{
	lvInput->SelectAll();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miRenameImageFilesClick(TObject *Sender)
{
	if (!jobQueue->isEmpty())
	{
		ShowMessage("Cannot modify list while converting");
		return;
	}
	if (!lvInput->Selected)
		return;
	int id = lvInput->Selected->Index;
	SourceFile *selectedFile = sourceFiles[id];	
	frmRename->ShowModal(selectedFile);	
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miSkipPackingBackClick(TObject *Sender)
{
    miSkipPackingBack->Checked = !miSkipPackingBack->Checked;
	for (int i=0; i<lvInput->Items->Count; i++)
	{
		TListItem *item = lvInput->Items->Item[i];
		if (item->Selected)
		{
			SourceFile *source = sourceFiles[i];
			source->doNotPack = miSkipPackingBack->Checked;
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::popupInputPopup(TObject *Sender)
{
	bool packingSkipped = false;
	for (int i=0; i<lvInput->Items->Count; i++)
	{
		TListItem *item = lvInput->Items->Item[i];
		if (item->Selected)
		{
			SourceFile *source = sourceFiles[i];
			if (source->doNotPack)
			{
				packingSkipped = true;
			}
			else
			{
				packingSkipped = false;
				break;
			}
		}
	}
	miSkipPackingBack->Checked = packingSkipped;
}
//---------------------------------------------------------------------------

