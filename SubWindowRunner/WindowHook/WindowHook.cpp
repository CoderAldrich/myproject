// WindowHook.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <atlstr.h>

#include <detours.h>
#pragma comment(lib,"detours.lib")
#include <atltypes.h>

#include "TerminateTestProcess.h"

static HWND hTopFrameWnd = NULL;


//CRect rcNewWin(0,0,1366,768);
CRect rcNewWin(-1366,-768,500,500);

BOOL (WINAPI *pShowWindow)(
						   __in HWND hWnd,
						   __in int nCmdShow
						   ) = ShowWindow;
BOOL WINAPI MyShowWindow(
						 __in HWND hWnd,
						 __in int nCmdShow
						 )
{
	CString strWinText;
	GetWindowTextW(hWnd,strWinText.GetBuffer(MAX_PATH),MAX_PATH);
	strWinText.ReleaseBuffer();
	if ( strWinText.Find(L"Debug") >= 0 )
	{
// 		__asm
// 		{
// 			int 3;
// 		}
		return TRUE;
	}

	HWND hParent = GetParent(hWnd);
	if ( NULL == hParent )
	{
// 		HWND hTopWnd = (HWND)GetPrivateProfileIntW(L"Config",L"TopWnd",0,L"C:\\myconfig.txt");
// 		SetParent( hWnd , hTopWnd );
// 
// 		PostMessage(hTopWnd,WM_USER+2222,(WPARAM)hWnd,NULL);

		if ( NULL == hTopFrameWnd )
		{
			hTopFrameWnd = hWnd;
		}

		MoveWindow(hWnd,rcNewWin.left,rcNewWin.top,rcNewWin.Width(),rcNewWin.Height(),TRUE);

	}

	BOOL TReturn = pShowWindow(hWnd,nCmdShow);
	return TReturn;
};


HWND (WINAPI *pCreateWindowExW)(
								__in DWORD dwExStyle,
								__in_opt LPCWSTR lpClassName,
								__in_opt LPCWSTR lpWindowName,
								__in DWORD dwStyle,
								__in int X,
								__in int Y,
								__in int nWidth,
								__in int nHeight,
								__in_opt HWND hWndParent,
								__in_opt HMENU hMenu,
								__in_opt HINSTANCE hInstance,
								__in_opt LPVOID lpParam
								) = CreateWindowExW;
HWND WINAPI MyCreateWindowExW(
							  __in DWORD dwExStyle,
							  __in_opt LPCWSTR lpClassName,
							  __in_opt LPCWSTR lpWindowName,
							  __in DWORD dwStyle,
							  __in int X,
							  __in int Y,
							  __in int nWidth,
							  __in int nHeight,
							  __in_opt HWND hWndParent,
							  __in_opt HMENU hMenu,
							  __in_opt HINSTANCE hInstance,
							  __in_opt LPVOID lpParam
							  )
{

	if ( NULL ==  hWndParent )
	{
 		return pCreateWindowExW(
 			dwExStyle|WS_EX_TOOLWINDOW,
 			lpClassName,
 			lpWindowName,
 			dwStyle,
 			rcNewWin.left,
			rcNewWin.top,
			rcNewWin.Width(),
			rcNewWin.Height(),
 			hWndParent,
 			hMenu,
 			hInstance,
 			lpParam
 			);
	}

	HWND TReturn = pCreateWindowExW(
		dwExStyle,
		lpClassName,
		lpWindowName,
		dwStyle,
		X,
		Y,
		nWidth,
		nHeight,
		hWndParent,
		hMenu,
		hInstance,
		lpParam
		);
	return TReturn;
};

LONG WINAPI ExpFilter(struct _EXCEPTION_POINTERS *pExp)
{
	return EXCEPTION_EXECUTE_HANDLER;
}

#include <atlstr.h>
#include <atlimage.h>

BOOL bCanCreateProcess = TRUE;

DWORD WINAPI PrintWindowThread( PVOID pParam )
{
#if defined(DEBUG) || defined(_DEBUG)
	Sleep(5000);
#else
	Sleep(20000);
#endif
	

	if (hTopFrameWnd)
	{
		RECT rcWin;
		GetWindowRect(hTopFrameWnd,&rcWin);

		HDC hDc = ::GetWindowDC(hTopFrameWnd);

		HDC hPaintDC = CreateCompatibleDC( hDc );
		HBITMAP hPaintBmp = CreateCompatibleBitmap( hDc ,rcWin.right - rcWin.left,rcWin.bottom - rcWin.top );
		::SelectObject(hPaintDC,hPaintBmp);
		
		//::InvalidateRect(hTopFrameWnd,CRect(0,0,rcNewWin.Width(),rcNewWin.Height()),TRUE);
 		//BitBlt(hPaintDC,0,0,rcNewWin.Width(),rcNewWin.Height(),hDc,0,0,SRCCOPY);

		::ReleaseDC(hTopFrameWnd,hDc);
		hDc = NULL;

		

		//SendMessage(hTopFrameWnd,WM_PAINT,(WPARAM)hPaintDC,0);
		PrintWindow(hTopFrameWnd,hPaintDC,0);
		
		int nIndex = GetPrivateProfileIntW(L"Config",L"Index",0,L"C:\\print.cfg");

		CString strFileName;
		strFileName.Format(L"C:\\test\\images\\img%d.jpg",nIndex);

		nIndex++;
		CString strTemp;
		strTemp.Format(L"%d",nIndex);
		WritePrivateProfileStringW(L"Config",L"Index",strTemp,L"C:\\print.cfg");

		CImage imgsaveer;
		imgsaveer.Attach(hPaintBmp);
		imgsaveer.Save(strFileName);

		DeleteObject(hPaintBmp);
		DeleteDC(hPaintDC);

		bCanCreateProcess = FALSE;
		::SetUnhandledExceptionFilter(ExpFilter);

		TerminateSystem();

	}
	return 0;
}


BOOL (WINAPI *pCreateProcessW)(
							   __in_opt LPCWSTR lpApplicationName,
							   __inout_opt LPWSTR lpCommandLine,
							   __in_opt LPSECURITY_ATTRIBUTES lpProcessAttributes,
							   __in_opt LPSECURITY_ATTRIBUTES lpThreadAttributes,
							   __in BOOL bInheritHandles,
							   __in DWORD dwCreationFlags,
							   __in_opt LPVOID lpEnvironment,
							   __in_opt LPCWSTR lpCurrentDirectory,
							   __in LPSTARTUPINFOW lpStartupInfo,
							   __out LPPROCESS_INFORMATION lpProcessInformation 
							   ) = CreateProcessW;
BOOL WINAPI MyCreateProcessW(
							 __in_opt LPCWSTR lpApplicationName,
							 __inout_opt LPWSTR lpCommandLine,
							 __in_opt LPSECURITY_ATTRIBUTES lpProcessAttributes,
							 __in_opt LPSECURITY_ATTRIBUTES lpThreadAttributes,
							 __in BOOL bInheritHandles,
							 __in DWORD dwCreationFlags,
							 __in_opt LPVOID lpEnvironment,
							 __in_opt LPCWSTR lpCurrentDirectory,
							 __in LPSTARTUPINFOW lpStartupInfo,
							 __out LPPROCESS_INFORMATION lpProcessInformation 
							 )
{
	if ( FALSE == bCanCreateProcess )
	{
		return TRUE;
	}
	
	CString strThisExePath;
	GetModuleFileNameW(NULL,strThisExePath.GetBuffer(MAX_PATH),MAX_PATH);
	strThisExePath.ReleaseBuffer();

	if ( strThisExePath.CompareNoCase(lpApplicationName) != 0 )
	{
		return TRUE;
	}

	BOOL TReturn = pCreateProcessW(
		lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation
		);
	return TReturn;
};



VOID StartHookShowWindow()
{
	BOOL SetSlient();
	SetSlient();

	::SetUnhandledExceptionFilter(ExpFilter);

	
	//禁止获取焦点
	LockSetForegroundWindow( LSFW_LOCK	);

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach((PVOID *)&pShowWindow,(PVOID)MyShowWindow);
	DetourAttach((PVOID *)&pCreateWindowExW,(PVOID)MyCreateWindowExW);
	DetourAttach((PVOID *)&pCreateProcessW,(PVOID)MyCreateProcessW);
	DetourTransactionCommit();

	CreateThread(NULL,0,PrintWindowThread,NULL,0,NULL);

	CString strThisPid;
	strThisPid.Format(L"%x",GetCurrentProcessId()*5);

	HANDLE hMutext = CreateMutex(NULL,FALSE,strThisPid);


}