//---------------------------------------------------------------------------


#pragma hdrstop

#include "FileNameMatch.h"
#include <SysUtils.hpp>
#include <Classes.hpp>

//---------------------------------------------------------------------------

#pragma package(smart_init)


bool MatchPattern(char *element, char *pattern)
{
	if( 0 == StrComp(pattern, "*") )
		return true;

	if( (*element == '\0') && (*pattern != '\0') )
		return false;

	if( *element == '\0' )
		return true;

	switch( *pattern )
	{
        case '*':
            if( MatchPattern(element, &pattern[1]) )
                return true;
            else
                return MatchPattern(&element[1], pattern);

        case '?':
            return MatchPattern(&element[1], &pattern[1]); 
    }

    if( *element == *pattern )
        return MatchPattern(&element[1], &pattern[1]);
    else
        return false;
}

bool MatchMultiplePatterns(AnsiString str, AnsiString patterns)
{
	TStringList *list = new TStringList();
	ExtractStrings(TSysCharSet() << ';', TSysCharSet(), patterns.c_str(), list);
	for (int i=0; i<list->Count; i++)
	{
		if (MatchPattern(str.c_str(), list->Strings[i].c_str()))
		{
			delete list;
			return true;
		}
	}
	delete list;
	return false;
}
