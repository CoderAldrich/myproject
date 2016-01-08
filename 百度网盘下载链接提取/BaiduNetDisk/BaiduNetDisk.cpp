
// ShuaClient.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "BaiduNetDisk.h"
#include "MainFrm.h"

#include "VirtualMouse.h"
#include "BrowserHelpFun.h"

#include "SAStatusLog.h"

#include "PublicFun.h"


#include <string>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


VOID MyParseCommandLine( 
					  CString &strUrl,
					  CString &strCheckCoed,
					  CString &strResSave,
					  CString &strShow
					  )
{
	LPWSTR *szArglist = NULL;  //�������ַ���ָ��,szArglist[i]�����i���ַ�������
	int nArgs = 0; //nArgs�����в����ĸ���  
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);//�����в�����������

	if(szArglist!= NULL)   
	{
		USES_CONVERSION; //���ͷ�ļ�����ĺ�
		for (int i=0;i<nArgs;i++)
		{
			CString strCmdPart;
			strCmdPart=W2CT(szArglist[i]);//LPWSTRת��ΪLPCSTR

			if (strCmdPart.GetAt(0) == L'-')
			{
				if(strCmdPart.CompareNoCase(L"-url") == 0)
				{
					if( i+1 < nArgs )
					{
						strUrl = szArglist[i+1];
						i++;
					}

				}
				
				if(strCmdPart.CompareNoCase(L"-code") == 0)
				{
					if( i+1 < nArgs )
					{
						strCheckCoed = szArglist[i+1];
						i++;
					}
				}
				
				if(strCmdPart.CompareNoCase(L"-res") == 0)
				{
					if( i+1 < nArgs )
					{
						strResSave = szArglist[i+1];
						i++;
					}
				}

				if(strCmdPart.CompareNoCase(L"-show") == 0)
				{
					if( i+1 < nArgs )
					{
						strShow = szArglist[i+1];
						i++;
					}
				}

			}
		}
		LocalFree(szArglist);  
	} 

}


BEGIN_MESSAGE_MAP(CBaiduNetDiskApp, CWinApp)

END_MESSAGE_MAP()


// CShuaClientApp ����

CBaiduNetDiskApp::CBaiduNetDiskApp()
{

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

// Ψһ��һ�� CShuaClientApp ����

CBaiduNetDiskApp theApp;

CSAStatusLog g_Loger(L"log");


BOOL SetExceptionCatcher();

CString g_strCheckCode;
CString g_strResSave;


BOOL CBaiduNetDiskApp::InitInstance()
{
	SetExceptionCatcher();

	StartVirtualMouse();
	RegisterBrowserEmulationMode(TRUE);
	BrowserFix();

	SetPriorityClass(GetCurrentProcess(),IDLE_PRIORITY_CLASS);


	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxOleInit())
	{
		return FALSE;
	}
	AfxEnableControlContainer();

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	CString strUrl;
	CString strCheckCoed;
	CString strResSave;
	CString strShow;
	MyParseCommandLine( 
		strUrl,
		strCheckCoed,
		strResSave,
		strShow
		);

	if ( strUrl.GetLength() == 0 || strCheckCoed.GetLength() == 0 || strResSave.GetLength() == 0)
	{
		AfxMessageBox(L"����������\r\n -url �ٶ���������\r\n-code ��֤��\r\n-res ������ȡ�������ӵ��ļ�\r\n -show [1|0]");

		return FALSE;
	}

	g_strCheckCode = strCheckCoed;
	g_strResSave = strResSave;

	CMainFrame *pMainFrame = new CMainFrame;


	pMainFrame->CreateInstance();

	CBaiduNetDiskView *pView = (CBaiduNetDiskView *)(pMainFrame->m_pView);
	
	if (pView)
	{
		pView->AutoStartWork();
		pView->Navigate(strUrl);
	}

	m_pMainWnd = pMainFrame;
	
	if( strShow == L"1" )
	{
		m_pMainWnd->ShowWindow(SW_SHOW);
	}
	else
	{
		LockSetForegroundWindow( LSFW_LOCK );
	}
	
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

int CBaiduNetDiskApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
