// FileNavigator.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include <Shlwapi.h>
#include <atlstr.h>
#include <detours.h>
#ifdef _WIN64
#pragma comment(lib,"detours64.lib")
#else
#pragma comment(lib,"detours.lib")
#endif


void pinyin_gb2312(char * inbuf, char * outbuf, bool m_blnSimaple = false, bool polyphone_support = false, bool m_blnFirstBig = false, bool m_blnAllBiG = false, bool m_LetterEnd = false, bool m_unknowSkip = true, bool m_filterPunc = true);
BOOL (WINAPI *pIntlStrEqWorkerW)(
								 BOOL fCaseSens,
								 LPCWSTR lpString1,
								 LPCWSTR lpString2,
								 int nChar
								 ) = IntlStrEqWorkerW;
BOOL WINAPI MyIntlStrEqWorkerW(
							   BOOL fCaseSens,
							   LPCWSTR lpString1,
							   LPCWSTR lpString2,
							   int nChar
							   )
{


	CString strShortPinYin;
	CString strFullPinYin;
	CStringA strTempPinYin;

	pinyin_gb2312(CStringA(lpString1).GetBuffer(), strTempPinYin.GetBuffer(200), false, false, true, true, true);
	strTempPinYin.ReleaseBuffer();
	strFullPinYin = strTempPinYin;
	strFullPinYin.MakeLower();

	BOOL TReturn = pIntlStrEqWorkerW(
		fCaseSens,
		strFullPinYin/*lpString1*/,
		lpString2,
		nChar
		);
	return TReturn;
};


VOID StartFastNavigator()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach((PVOID *)&pIntlStrEqWorkerW,MyIntlStrEqWorkerW);
	DetourTransactionCommit();
}