// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <atlstr.h>

BOOL g_bHookThisProcess =  FALSE;
VOID UnHookWindow();
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			WCHAR szLocalPath[MAX_PATH]={0};
			GetModuleFileNameW(NULL,szLocalPath,MAX_PATH);
			CString strExePath;
			strExePath = szLocalPath;

			if (strExePath.CompareNoCase(L"C:\\Windows\\explorer.exe") == 0)
			{
				g_bHookThisProcess = TRUE;
			}

		}
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		{
			if (g_bHookThisProcess)
			{
				UnHookWindow();
			}
			
		}
		break;
	}
	return TRUE;
}

