//---------------------------------------------------------------------------

#ifndef FileNameMatchH
#define FileNameMatchH
//---------------------------------------------------------------------------

#include <System.hpp>

bool MatchPattern(char *element, char *pattern);

bool MatchMultiplePatterns(AnsiString str, AnsiString patterns);

#endif
