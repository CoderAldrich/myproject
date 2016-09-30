#include "stdafx.h"
#include "HelpFun.h"

int DivisionString(CStringA strSeparate, CStringA strSourceString, CStringA * pStringArray, int nArrayCount)
{

	if(
		strSeparate.GetLength() == 0 ||
		strSourceString.GetLength() == 0||
		pStringArray == NULL || 
		nArrayCount <=1 
		)
	{
		return 0;
	}

	int nCount = 0;
	while(true)
	{
		int nEnd = strSourceString.Find(strSeparate,0);
		if ( nEnd >= 0 )
		{
			pStringArray[nCount] = strSourceString.Left(nEnd);
			nCount++;

			strSourceString = strSourceString.Right(strSourceString.GetLength() - nEnd-strSeparate.GetLength());
		}
		else
		{
			pStringArray[nCount] = strSourceString;
			nCount++;
			strSourceString = "";

			break;
		}

		if ( nCount >= nArrayCount)
		{
			break;
		}
	}

	return nCount;
}


//调试信息输出

#if defined(DEBUG) || defined(_DEBUG)
#include <stdio.h>

#define DEBUG_STRING_BUFFER_LEN MAX_PATH*4

VOID DebugStringW(const WCHAR* fmt, ...)
{
	WCHAR mtBuff[DEBUG_STRING_BUFFER_LEN]={0};

	va_list argptr;
	va_start(argptr, fmt);//_vsntprintf
	int cbBuf = _vsntprintf_s(mtBuff, DEBUG_STRING_BUFFER_LEN,_TRUNCATE, fmt, argptr) + 1;
	WCHAR *lpBuf = new WCHAR[cbBuf+10];
	_vsnwprintf_s(lpBuf, cbBuf, _TRUNCATE,fmt, argptr);
	wcscat_s(lpBuf,cbBuf+10,L"\n");
	OutputDebugStringW(lpBuf);

	delete lpBuf;
}


VOID DebugStringA(const CHAR* fmt, ...)
{
	CHAR mtBuff[DEBUG_STRING_BUFFER_LEN*10]={0};

	va_list argptr;
	va_start(argptr, fmt);//_vsntprintf
	int cbBuf = _vsnprintf_s(mtBuff, DEBUG_STRING_BUFFER_LEN,_TRUNCATE, fmt, argptr) + 1;
	CHAR *lpBuf = new CHAR[cbBuf+10];
	_vsnprintf_s(lpBuf, cbBuf, _TRUNCATE,fmt, argptr);
	strcat_s(lpBuf,cbBuf+10,"\n");
	OutputDebugStringA(lpBuf);

	delete lpBuf;
}

#endif

