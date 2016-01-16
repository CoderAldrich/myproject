// NavigateFile.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "NavigateFile.h"
#include <atlstr.h>
#include <OleAcc.h>
#pragma comment(lib,"Oleacc.lib")


//#include "HzToPy.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	system("taskkill /f /im explorer.exe");
	system("taskkill /f /im werfault.exe");
	system("start explorer.exe");

	Sleep(3000);

	HMODULE hWinHook = LoadLibrary(L"WinHook.dll");
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

// 	OleInitialize(NULL);
// 	IAccessible *paccMainWindow = NULL;
// 	HRESULT hr = AccessibleObjectFromWindow((HWND)0x000a073a,OBJID_WINDOW,IID_IAccessible,(void **)&paccMainWindow);
// 
// 	CHzToPy Test;
// 
// 	if (hr == S_OK && paccMainWindow)
// 	{
// 		LIST_ENUM_RES Res;
// 		FindChild(paccMainWindow,FALSE,&Res);
// 		paccMainWindow->Release();
// 
// 		for (LIST_ENUM_RES_PTR it = Res.begin();it!=Res.end();it++)
// 		{
// 			CString strPinYin;
// 			strPinYin = Test.HzToPinYin(it->strItemName);
// 			if (strPinYin.GetLength() == 0 )
// 			{
// 				strPinYin = it->strItemName;
// 			}
// 			strPinYin.MakeLower();
// 
// 			if (strPinYin.Find(L"file") >=0)
// 			{	
// 				CComVariant vtNull;
// 				vtNull = CHILDID_SELF;
// 				it->pAcc->accSelect(SELFLAG_TAKESELECTION,vtNull);
// 				Sleep(110);
// 			}
// 
// 			it->pAcc->Release();
// 			
// 		}
// 
// 	}

	return (int) 0;
}

