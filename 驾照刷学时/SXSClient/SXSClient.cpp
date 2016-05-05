
// ShuaClient.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "SXSClient.h"
#include "MainFrm.h"

#include "BrowserHelpFun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CShuaClientApp

BEGIN_MESSAGE_MAP(CSXSClientApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CSXSClientApp::OnAppAbout)
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// ��׼��ӡ��������
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CShuaClientApp ����

CSXSClientApp::CSXSClientApp()
{

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

// Ψһ��һ�� CShuaClientApp ����

CSXSClientApp theApp;


BOOL SetSlient();

VOID MyParseCommandLine(
					  LPCWSTR pszRunCmd,
					  CString &strUserName,
					  CString &strPassWord
					  )
{
	LPWSTR *szArglist = NULL;  //�������ַ���ָ��,szArglist[i]�����i���ַ�������
	int nArgs = 0; //nArgs�����в����ĸ���  
	szArglist = CommandLineToArgvW(pszRunCmd, &nArgs);//�����в�����������
	if(szArglist!= NULL)   
	{
		for (int i=0;i<nArgs;i++)
		{
			CString strCmdPart;
			strCmdPart=W2CT(szArglist[i]);//LPWSTRת��ΪLPCSTR
			if(strCmdPart.CompareNoCase(L"-username") == 0)
			{
				if( i+1 < nArgs )
				{
					strUserName = szArglist[i+1];
					i++;
				}
			}

			if(strCmdPart.CompareNoCase(L"-password") == 0)
			{
				if( i+1 < nArgs )
				{
					strPassWord = szArglist[i+1];
					i++;
				}
			}
		}
		LocalFree(szArglist);  
	}
}


BOOL CSXSClientApp::InitInstance()
{

	SetSlient();
	RegisterBrowserEmulationMode(TRUE);
	BrowserFix();

	MyParseCommandLine(GetCommandLineW(),theApp.m_strUserName,theApp.m_strPassWord);

	int a=0;

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

	// ������׼������DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	CMainFrame *pMainFrame = new CMainFrame;
	pMainFrame->CreateInstance(L"http://www.130100.prcjx.cn/");

	m_pMainWnd = pMainFrame;

	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// �������к�׺ʱ�ŵ��� DragAcceptFiles
	//  �� SDI Ӧ�ó����У���Ӧ�� ProcessShellCommand ֮����
	return TRUE;
}



// �������жԻ����Ӧ�ó�������
void CSXSClientApp::OnAppAbout()
{
}

// CShuaClientApp ��Ϣ�������




int CSXSClientApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
