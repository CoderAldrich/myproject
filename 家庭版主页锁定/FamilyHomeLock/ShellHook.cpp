#include "stdafx.h"
#include <winuser.h>
#include "ShellHook.h"

static ShellHookCallBack g_pCallBack = NULL;
static DWORD g_dwHookType = 0;

LRESULT CALLBACK ShellHookWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	const UINT dwShellHookMsg =RegisterWindowMessage(TEXT("SHELLHOOK"));

	if (dwShellHookMsg == message)
	{
		if( g_dwHookType&wParam)
		{
			if (g_pCallBack)
			{
				g_pCallBack(hWnd,wParam,lParam);
			}
		}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


DWORD WINAPI ShellHookThread(PVOID pParam)
{

	HWND hWnd = CreateWindowW(L"Button", NULL, WS_OVERLAPPED,0, 0, 10, 10, HWND_MESSAGE, NULL, NULL, NULL);

	SetWindowLongW(hWnd,GWL_WNDPROC,(LONG)ShellHookWndProc);

	BOOL bRes = RegisterShellHookWindow(hWnd);

	MSG msg;
	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

BOOL StartShellHook(  DWORD dwHookType,ShellHookCallBack pCallBack )
{
	BOOL bRes = FALSE;
	static BOOL bStart = FALSE;
	if ( FALSE == bStart )
	{
		bStart = TRUE;

		g_pCallBack = pCallBack;
		g_dwHookType = dwHookType;
		bRes = CreateThread( NULL , 0 , ShellHookThread ,0,0,NULL ) != NULL;
	}
	
	return bRes;
}