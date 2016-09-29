
// SubWindowRunner.cpp : ����Ӧ�ó��������Ϊ��
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


// CSubWindowRunnerApp ����

CSubWindowRunnerApp::CSubWindowRunnerApp()
{

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

// Ψһ��һ�� CSubWindowRunnerApp ����

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


//���������в���
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
			if ( wChar == L'-'|| i+1==nCmdLen/*���һ���ַ�*/)
			{
				bInParamName = TRUE;
				bInParamValue = FALSE;

				if ( i+ 1== nCmdLen )//��������һ���ַ�
				{
					strTempParamValue+=wChar;
				}

				strTempParamValue.Trim();

				//�˴����������в���
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
	//��ֹ��ȡ����
	LockSetForegroundWindow( LSFW_LOCK	);

	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	// ��Ҫ���������ڣ��˴��뽫�����µĿ�ܴ���
	// ����Ȼ��������ΪӦ�ó���������ڶ���
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// ���������ؿ�ܼ�����Դ
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);

	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	CString strWnd;
	strWnd.Format(L"%d",pFrame->m_hWnd);
	WritePrivateProfileStringW(L"Config",L"TopWnd",strWnd,L"C:\\myconfig.txt");

	//�Թ�����ʽ��������
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

		//ע��������DLL
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

