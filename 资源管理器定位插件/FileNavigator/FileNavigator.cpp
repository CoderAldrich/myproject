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

HHOOK g_hCbtHook = NULL;

//获取当前模块句柄
HMODULE ModuleHandleByAddr(const void* ptrAddr)  
{  
	MEMORY_BASIC_INFORMATION info;  
	::VirtualQuery(ptrAddr, &info, sizeof(info));  
	return (HMODULE)info.AllocationBase;  
}  
/*  
功能：获取当前模块句柄
返回值：当前模块句柄
*/  
HMODULE ThisModuleHandle()  
{  
	static HMODULE sInstance = ModuleHandleByAddr((void*)&ThisModuleHandle);  
	return sInstance;  
}

LRESULT CALLBACK CbtHookProc(int code, WPARAM wParam, LPARAM lParam)
{
	return CallNextHookEx(g_hCbtHook,code,wParam,lParam);
}
BOOL InstallHook()
{
	g_hCbtHook = SetWindowsHookExW(WH_CBT,CbtHookProc,ThisModuleHandle(),0);

	return TRUE;
}

static BOOL bInExplorer = FALSE;
static BOOL bHooked = FALSE;

VOID StartFastNavigator()
{
	CString strThisModulePath;
	GetModuleFileName(NULL,strThisModulePath.GetBuffer(MAX_PATH),MAX_PATH);
	strThisModulePath.ReleaseBuffer();
	if (strThisModulePath.Right(12).CompareNoCase(L"explorer.exe") == 0 )
	{
		bInExplorer = TRUE;
	}

	if( bInExplorer && FALSE == bHooked )
	{
		OutputDebugStringW(L"Start Api Hook");
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach((PVOID *)&pIntlStrEqWorkerW,MyIntlStrEqWorkerW);
		DetourTransactionCommit();

		bHooked = TRUE;
	}
}

VOID StopFastNavigator()
{
	if(bHooked)
	{
		OutputDebugStringW(L"Stop Api Hook");
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach((PVOID *)&pIntlStrEqWorkerW,MyIntlStrEqWorkerW);
		DetourTransactionCommit();

		bHooked = FALSE;
	}
}