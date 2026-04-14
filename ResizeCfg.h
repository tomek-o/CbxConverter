//---------------------------------------------------------------------------

#ifndef ResizeCfgH
#define ResizeCfgH
//---------------------------------------------------------------------------

struct ResizeCfg
{
    enum Mode {
		ModeRegular = 0,
		ModeOnOversize
	} mode;
	enum ModeOnOversizeSub {
		ModeOnOversizeWiderOrHigher = 0,
		ModeOnOversizeWider,
		ModeOnOversizeHigher
	} modeOnOversize;
	int resizePct;
	int resizeThreshold;
	int resizeTarget;
	ResizeCfg(void):
		mode(ModeRegular),
		modeOnOversize(ModeOnOversizeWiderOrHigher),
		resizePct(100),
		resizeThreshold(1366),
		resizeTarget(1366)
	{}
};

#endif
