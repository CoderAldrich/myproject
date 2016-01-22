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


//    对应Unicode的调试输出
void WINAPI MyDbgPrintW(LPCTSTR strFormat, ...)
{
#ifdef _DEBUG
	const int BUFFER_SIZE = 12800;
	va_list args = NULL;
	va_start(args, strFormat);
	WCHAR szBuffer[BUFFER_SIZE] = { 0 };
	_vsntprintf_s(szBuffer, BUFFER_SIZE, strFormat, args);//_vsnwprintf for Unicode
	va_end(args);
	::OutputDebugString(szBuffer);
#endif
}
//    对应ASCII的调试输出
void WINAPI MyDbgPrintA(const char* strFormat, ...)
{
#ifdef _DEBUG
	const int BUFFER_SIZE = 12800;
	char   szBuffer [BUFFER_SIZE] = {0};
	va_list args = NULL;
	va_start (args, strFormat) ;
	_vsnprintf_s ( szBuffer, BUFFER_SIZE, strFormat, args) ;
	va_end (args);
	::OutputDebugStringA (szBuffer);
#endif // _DEBUG
}
