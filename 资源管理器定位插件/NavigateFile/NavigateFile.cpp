// NavigateFile.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "NavigateFile.h"
#include <atlstr.h>
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{

	HANDLE hMutex=CreateMutex(NULL,TRUE,L"NavigateFile");
	if(hMutex)
	{
		if(ERROR_ALREADY_EXISTS==GetLastError())
		{
			return FALSE;
		}
	}

	HMODULE hWinHook = LoadLibrary(L"WinHookEx.dll");
	if (hWinHook)
	{
		typedef BOOL (*TypeInstallHook)();
		TypeInstallHook pInstallHook = (TypeInstallHook)GetProcAddress(hWinHook,"InstallHook");
		if (pInstallHook)
		{
			pInstallHook();
		}
	}

	MSG msg;
	while (GetMessage(&msg,0,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) 0;
}

