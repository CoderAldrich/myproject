// NavigatorRun.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "NavigatorRun.h"


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	HMODULE hWinHook = LoadLibrary(L"FileNavigator.dll");
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

	return (int) msg.wParam;
}

