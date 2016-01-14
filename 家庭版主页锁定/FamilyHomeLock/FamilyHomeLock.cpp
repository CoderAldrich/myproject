// FamilyHomeLock.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "FamilyHomeLock.h"
#include "LockApi.h"
#include "ShellHook.h"
#include "RestartBrowserLock.h"
VOID CALLBACK MyShellHookCallBack(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	if ( HSHELL_WINDOWCREATED == wParam)
	{
		static DWORD dwLastRestartTime = 0;

		HWND hWnd = (HWND)lParam;

		WCHAR szClassName[MAX_PATH];
		GetClassNameW(hWnd,szClassName,MAX_PATH);

#if defined(DEBUG) || defined(_DEBUG)
		OutputDebugStringW(szClassName);
		OutputDebugStringW(L"\n");
#endif

		if ( FALSE == CheckWBClsName(szClassName) )
		{
			return ;
		}

		if ( dwLastRestartTime == 0 || GetTickCount() - dwLastRestartTime > 3000 )
		{
			DWORD dwProcessID = 0;
			DWORD dwThreadID = GetWindowThreadProcessId(hWnd,&dwProcessID);
			BOOL bRestart = RestartBrowser(dwProcessID);

			if ( bRestart )
			{
#ifdef DEBUG
				OutputDebugStringW(L"干掉这个进程并重启\n");
#endif
				dwLastRestartTime = GetTickCount();
			}
		}
	}
}

BOOL MainRun()
{
	InitLockApi();
	StartShellHook(HSHELL_WINDOWCREATED,MyShellHookCallBack);
	return TRUE;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	MainRun();
	while (1)
	{
		Sleep(1000);
	}
	return (int) 0;
}