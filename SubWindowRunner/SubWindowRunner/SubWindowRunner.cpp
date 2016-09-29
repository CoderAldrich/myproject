
// SubWindowRunner.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "SubWindowRunner.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(CSubWindowRunnerApp, CWinApp)
END_MESSAGE_MAP()


// CSubWindowRunnerApp 构造

CSubWindowRunnerApp::CSubWindowRunnerApp()
{

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CSubWindowRunnerApp 对象

CSubWindowRunnerApp theApp;


BOOL InjectDll_RemoteThread(DWORD ProcessID,LPCWSTR szDllPath)
{
	HANDLE hProcessHandle = OpenProcess(PROCESS_VM_OPERATION|PROCESS_VM_WRITE|PROCESS_CREATE_THREAD|PROCESS_QUERY_INFORMATION|PROCESS_VM_OPERATION|PROCESS_VM_WRITE|PROCESS_VM_READ,FALSE,ProcessID);

	if (hProcessHandle)
	{
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

	return FALSE;
}


//解析命令行参数
VOID MyParseCommandLine(  LPCWSTR pszRunCmd, CString &strExeFilePath,CString &strTestUrl)
{
	CString strRunCmd;
	strRunCmd = pszRunCmd;
	int nCmdLen = strRunCmd.GetLength();

	BOOL bInParamName = FALSE;
	BOOL bInParamValue = FALSE;
	CString strTempParamName;
	CString strTempParamValue;

	for ( int i=0;i<nCmdLen;i++ )
	{
		WCHAR wChar = strRunCmd.GetAt(i);

		if ( FALSE == bInParamName )
		{
			if ( wChar == L'-'|| i+1==nCmdLen/*最后一个字符*/)
			{
				bInParamName = TRUE;
				bInParamValue = FALSE;

				if ( i+ 1== nCmdLen )//如果是最后一个字符
				{
					strTempParamValue+=wChar;
				}

				strTempParamValue.Trim();

				//此处解析命令行参数
				if ( strTempParamName.CompareNoCase(L"-file") == 0 )
				{
					strExeFilePath = strTempParamValue;
				}

				if ( strTempParamName.CompareNoCase(L"-url") == 0 )
				{
					strTestUrl = strTempParamValue;
				}

				strTempParamName = L"";
				strTempParamValue = L"";
			}
		}

		if ( bInParamName )
		{
			if (wChar == L' ')
			{
				bInParamName = FALSE;
				bInParamValue = TRUE;
				strTempParamValue = L"";
			}
			else
			{
				strTempParamName+=wChar;
			}
		}

		if ( bInParamValue )
		{
			strTempParamValue+=wChar;
		}

	}
}


BOOL CSubWindowRunnerApp::InitInstance()
{
	//禁止获取焦点
	LockSetForegroundWindow( LSFW_LOCK	);

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	// 若要创建主窗口，此代码将创建新的框架窗口
	// 对象，然后将其设置为应用程序的主窗口对象
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// 创建并加载框架及其资源
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	CString strWnd;
	strWnd.Format(L"%d",pFrame->m_hWnd);
	WritePrivateProfileStringW(L"Config",L"TopWnd",strWnd,L"C:\\myconfig.txt");

	//以挂起形式创建进程
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si,sizeof(si));
	si.cb = sizeof(si);

// 	si.dwFlags = STARTF_USESHOWWINDOW;
// 	si.wShowWindow = SW_HIDE;
	CString strExeFile;
	CString strTestUrl;
	MyParseCommandLine(GetCommandLineW(),strExeFile,strTestUrl);
	CString strCmdLine;
	strCmdLine = L"\""+strExeFile+L"\" "+strTestUrl;

	BOOL bRes = CreateProcess(strExeFile, strCmdLine.GetBuffer(),NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&si,&pi);
	if ( bRes )
	{
		WCHAR szLocalPath[MAX_PATH]={0};
		GetModuleFileNameW( NULL  ,szLocalPath,MAX_PATH);
		WCHAR *pPathEnd = (WCHAR *)szLocalPath+wcslen(szLocalPath);
		while (pPathEnd != szLocalPath && *pPathEnd != L'\\') pPathEnd--;
		*(pPathEnd+1) = 0;
		wcscat_s(szLocalPath,MAX_PATH,L"WindowHook.dll");

		//注入主进程DLL
		BOOL bRunRes = InjectDll_RemoteThread( pi.dwProcessId,szLocalPath);

		if ( FALSE == bRunRes )
		{
			TerminateProcess(pi.hProcess,0);
		}

		::ResumeThread(pi.hThread);

		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}


	return TRUE;
}

