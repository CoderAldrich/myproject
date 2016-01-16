// NavigateFile.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "NavigateFile.h"
#include <atlstr.h>

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{

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

