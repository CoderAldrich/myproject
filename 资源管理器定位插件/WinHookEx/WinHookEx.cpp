// WinHookEx.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "WinHookEx.h"
#include "ResultShowDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CWinHookExApp, CWinApp)
END_MESSAGE_MAP()

CWinHookExApp::CWinHookExApp()
{

}


// Ψһ��һ�� CWinHookExApp ����

CWinHookExApp theApp;
CResultShowDlg *g_pShowDlg = NULL;

BOOL g_bHookThisProcess = FALSE;
VOID UnHookWindow();
BOOL CWinHookExApp::InitInstance()
{
	CWinApp::InitInstance();

	WCHAR szLocalPath[MAX_PATH]={0};
	GetModuleFileNameW(NULL,szLocalPath,MAX_PATH);
	CString strExePath;
	strExePath = szLocalPath;

	if (strExePath.CompareNoCase(L"C:\\Windows\\explorer.exe") == 0)
	{
		g_bHookThisProcess = TRUE;
	}
	
	if (g_bHookThisProcess)
	{
		g_pShowDlg = new CResultShowDlg;
		g_pShowDlg->Create(IDD_DIALOG1);
	}

	return TRUE;
}

int CWinHookExApp::ExitInstance()
{
	UnHookWindow();

	return CWinApp::ExitInstance();
}
