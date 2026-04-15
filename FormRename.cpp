//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormRename.h"
#include "SourceFile.h"
#include "Log.h"
#include <stdio.h>
#include <set>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmRename *frmRename;
//---------------------------------------------------------------------------

__fastcall TfrmRename::TfrmRename(TComponent* Owner)
	: TForm(Owner),
	sourceFile(NULL),
	offset(0)
{
	renameType = static_cast<RenameType>(cbRenameType->ItemIndex);
	pattern = edPattern->Text;
}
//---------------------------------------------------------------------------

void TfrmRename::ShowModal(SourceFile *sourceFile)
{
	this->sourceFile = sourceFile;
	lvFiles->Items->Count = sourceFile->fileDescs.size();
	lvFiles->Invalidate();
	TForm::ShowModal();
}

void __fastcall TfrmRename::lvFilesData(TObject *Sender, TListItem *Item)
{
	int id = Item->Index;
	const SourceFile::FileDesc &fd = sourceFile->fileDescs[id];
	Item->Caption = ExtractFileName(fd.name);
	AnsiString newName = GetNewName(fd.name, id);
	if (newName != fd.name)
	{
    	Item->SubItems->Add(ExtractFileName(newName));
	}
}
//---------------------------------------------------------------------------

AnsiString TfrmRename::GetNewName(AnsiString name, int id)
{
	if (renameType == RenameExtractingNumberFromName)
	{
		AnsiString tmp = ExtractFileName(name);
		int pos = tmp.LastDelimiter(".");
		if (pos > 0)
		{
			tmp = tmp.SubString(1, pos-1);
		}
		for (int i=tmp.Length(); i>0; i--)
		{
			char c = tmp[i];
			if (!isdigit(c))
			{
				tmp = tmp.SubString(i+1, tmp.Length() - i);
				break;
			}
		}
		int number = 0;
		if (sscanf(tmp.c_str(), "%d", &number) == 1)
		{
			AnsiString ret;
			ret.sprintf(pattern.c_str(), number + offset);
			ret = ExtractFileDir(name) + "\\" + ret + ExtractFileExt(name);
			return ret;
		}
		else
		{
			LOG("Failed to extract number from %s\n", name.c_str());
			return "";
		}
	}
	else if (renameType == RenameStartingFromNumber)
	{
		AnsiString ret;
		ret.sprintf(pattern.c_str(), id + offset);
		ret = ExtractFileDir(name) + "\\" + ret + ExtractFileExt(name);
		return ret;
	}
	else
	{
    	return "Unhandled renaming type!";
	}
}

void __fastcall TfrmRename::cbRenameTypeChange(TObject *Sender)
{
	renameType = static_cast<RenameType>(cbRenameType->ItemIndex);
	lvFiles->Invalidate();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmRename::edOffsetChange(TObject *Sender)
{
	offset = StrToIntDef(edOffset->Text, 0);	
	lvFiles->Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmRename::edPatternChange(TObject *Sender)
{
	pattern = edPattern->Text;
	lvFiles->Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmRename::btnRenameClick(TObject *Sender)
{
	std::set<AnsiString> newNames;
	for (unsigned int i=0; i<sourceFile->fileDescs.size(); i++)
	{
		const SourceFile::FileDesc &fd = sourceFile->fileDescs[i];
		AnsiString newName = GetNewName(fd.name, i);
		if (newName == "")
		{
			AnsiString msg;
			msg.sprintf("Cannot rename files, cannot create new name for:\n%s", fd.name.c_str());
			MessageBox(this->Handle, msg.c_str(), this->Caption.c_str(), MB_ICONEXCLAMATION);
			return;
		}
		if (newNames.find(newName) != newNames.end())
		{
			LOG("Duplicated new name: %s\n", newName.c_str());
			AnsiString msg;
			msg.sprintf("Cannot rename files, duplicated name found among new names:\n%s", newName.c_str());
			MessageBox(this->Handle, msg.c_str(), this->Caption.c_str(), MB_ICONEXCLAMATION);
			return;
		}
		newNames.insert(newName);
	}

	for (unsigned int i=0; i<sourceFile->fileDescs.size(); i++)
	{
		SourceFile::FileDesc &fd = sourceFile->fileDescs[i];
		AnsiString newName = GetNewName(fd.name, i);
		if (RenameFile(fd.name, newName) == false)
		{
			AnsiString msg;
			msg.sprintf("Failed renaming\n%s\nto\n%s", fd.name.c_str(), newName.c_str());
			MessageBox(this->Handle, msg.c_str(), this->Caption.c_str(), MB_ICONEXCLAMATION);
			return;
		}
		else
		{
        	fd.name = newName;
		}
	}
	lvFiles->Invalidate();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmRename::miDeleteSelectedFilesClick(TObject *Sender)
{
	for (int i=lvFiles->Items->Count-1; i>=0; i--)
	{
		TListItem *item = lvFiles->Items->Item[i];
		if (item->Selected)
		{
			SourceFile::FileDesc &fd = sourceFile->fileDescs[i];
			DeleteFile(fd.name);
			sourceFile->fileDescs.erase(sourceFile->fileDescs.begin() + i);
		}
	}
	lvFiles->Items->Count = sourceFile->fileDescs.size();
	lvFiles->ClearSelection();
	lvFiles->Invalidate();
}
//---------------------------------------------------------------------------

