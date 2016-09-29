// WindowHook.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <detours.h>
#pragma comment(lib,"detours.lib")


BOOL (WINAPI *pShowWindow)(
						   __in HWND hWnd,
						   __in int nCmdShow
						   ) = ShowWindow;
BOOL WINAPI MyShowWindow(
						 __in HWND hWnd,
						 __in int nCmdShow
						 )
{
	HWND hParent = GetParent(hWnd);
	if ( NULL == hParent )
	{
		HWND hTopWnd = (HWND)GetPrivateProfileIntW(L"Config",L"TopWnd",0,L"C:\\myconfig.txt");
		SetParent( hWnd , hTopWnd );

		PostMessage(hTopWnd,WM_USER+2222,(WPARAM)hWnd,NULL);
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
			dwExStyle,
			lpClassName,
			lpWindowName,
			dwStyle|WS_CHILD,
			X,
			Y,
			nWidth,
			nHeight,
			(HWND)GetPrivateProfileIntW(L"Config",L"TopWnd",0,L"C:\\myconfig.txt"),
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


VOID StartHookShowWindow()
{
	BOOL SetSlient();

	SetSlient();

	//禁止获取焦点
	LockSetForegroundWindow( LSFW_LOCK	);

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach((PVOID *)&pShowWindow,(PVOID)MyShowWindow);
	DetourAttach((PVOID *)&pCreateWindowExW,(PVOID)MyCreateWindowExW);
	DetourTransactionCommit();
}