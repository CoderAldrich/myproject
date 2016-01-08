
// ShuaClient.cpp : 定义应用程序的类行为。
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
	LPWSTR *szArglist = NULL;  //命令行字符串指针,szArglist[i]代表第i个字符串变量
	int nArgs = 0; //nArgs命令行参数的个数  
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);//命令行参数解析函数

	if(szArglist!= NULL)   
	{
		USES_CONVERSION; //相关头文件定义的宏
		for (int i=0;i<nArgs;i++)
		{
			CString strCmdPart;
			strCmdPart=W2CT(szArglist[i]);//LPWSTR转换为LPCSTR

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


// CShuaClientApp 构造

CBaiduNetDiskApp::CBaiduNetDiskApp()
{

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CShuaClientApp 对象

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
		AfxMessageBox(L"启动参数：\r\n -url 百度网盘链接\r\n-code 验证码\r\n-res 保存提取到的链接的文件\r\n -show [1|0]");

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
