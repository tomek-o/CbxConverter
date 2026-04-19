#ifndef SourceFileH
#define SourceFileH

#include <vector>
#include <map>

#include "ResizeCfg.h"

struct SourceFile
{
	enum State {
		IDLE = 0,
		UNPACKING,
		RENDERING_PDF,
		IDENTIFYING,
		CONVERTING,
		PACKING,
		DONE,
		SOURCE_ERROR
	} state;

	int stateProgress;

	int jobCount;

	AnsiString stateName(void)
	{
		switch (state)
		{
			case IDLE:
				if (jobCount == 0)
					return "Idle";
				else
					return "Waiting...";
			case UNPACKING:
				return "Unpacking...";
			case RENDERING_PDF:
				return "Rendering PDF...";
			case IDENTIFYING:
				return (AnsiString)"Identifying, " + stateProgress + "%";
			case CONVERTING:
				return (AnsiString)"Converting, " + stateProgress + "%";
			case PACKING:
				return "Packing...";
			case DONE:
				return "Done";
			case SOURCE_ERROR:
				return "Error processing file";
			default:
				return "???";
		}
	}

	AnsiString name;
	AnsiString rootPath;
    AnsiString tmpDir;	
	uint64_t size;
	unsigned int imgCount;
	uint64_t imgSize;
	ResizeCfg resizeCfg;
	bool unpacked;
	bool doNotPack;
	bool defaultResizeApplied;

	bool widthValid;
	AnsiString asWidth;

	bool outSizeValid;
	uint64_t outSize;
	AnsiString outFile; 

	struct FileDesc {
		AnsiString name;
		int width;
		int height;
		uint64_t size;
		FileDesc(void):
			width(-1),
			height(-1),
			size(0)
		{}
    };
	std::vector<FileDesc> fileDescs;
	std::map<int, int> widths;
	int mostFrequentWidth;

	SourceFile(void) {
		size = 0;
		imgCount = 0;
		imgSize = 0;
		unpacked = false;
		doNotPack = false;
		widthValid = false;
		outSizeValid = false;
		outSize = 0;
		defaultResizeApplied = false;
		mostFrequentWidth = 0;
		state = IDLE;
		stateProgress = -1;
		jobCount = 0;
	}

	bool operator==(const SourceFile &b)
	{
    	return name == b.name;
	}


};

#endif
