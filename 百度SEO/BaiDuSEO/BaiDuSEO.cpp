
// ShuaClient.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "BaiDuSEO.h"
#include "MainFrm.h"

#include "VirtualMouse.h"
#include "BrowserHelpFun.h"
#include "PublicFun.h"

#include <detours.h>
#pragma comment(lib,"detours.lib")

#include <WinInet.h>
#pragma comment(lib,"wininet.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CBaiDuSEOApp, CWinApp)

END_MESSAGE_MAP()


// CShuaClientApp 构造

CBaiDuSEOApp::CBaiDuSEOApp()
{

}

// 唯一的一个 CShuaClientApp 对象

CBaiDuSEOApp theApp;

BOOL SetSlient();
BOOL SetExceptionCatcher();
BOOL g_bPhoneMode = TRUE;

//解析命令行参数
VOID MyParseCommandLine(
					  LPCWSTR pszRunCmd,
					  CString &strProxy,
					  CString &strKeyWord,
					  CString &strTargetUrl
					  )
{
	CString strRunCmd;
	strRunCmd = pszRunCmd;
	int nCmdLen = strRunCmd.GetLength();

	BOOL bInParamName = FALSE;
	BOOL bInParamValue = FALSE;
	CString strTempParamName;
	CString strTempParamValue;
	WCHAR chPreChar = 0;
	for ( int i=0;i<nCmdLen;i++ )
	{
		WCHAR wChar = strRunCmd.GetAt(i);

		if ( FALSE == bInParamName )
		{
			if ( ( wChar == L'-' && (chPreChar == 0 || chPreChar == L' ') ) || i+1==nCmdLen/*最后一个字符*/)
			{
				bInParamName = TRUE;
				bInParamValue = FALSE;

				if ( i+ 1== nCmdLen )//如果是最后一个字符
				{
					strTempParamValue+=wChar;
				}

				strTempParamValue.Trim();

				//此处解析命令行参数
				if ( strTempParamName.CompareNoCase(L"-proxy") == 0 )
				{
					strProxy = strTempParamValue;
				}

				if ( strTempParamName.CompareNoCase(L"-keyword") == 0 )
				{
					strKeyWord = strTempParamValue;
				}

				if ( strTempParamName.CompareNoCase(L"-targeturl") == 0 )
				{
					strTargetUrl = strTempParamValue;
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

		chPreChar=wChar;
	}
}

CString strHttpProxyInfo;

HINTERNET (WINAPI *pInternetOpenW)(
								   __in_opt LPCWSTR lpszAgent,
								   __in DWORD dwAccessType,
								   __in_opt LPCWSTR lpszProxy,
								   __in_opt LPCWSTR lpszProxyBypass,
								   __in DWORD dwFlags 
								   ) = InternetOpenW;
HINTERNET WINAPI MyInternetOpenW(
								 __in_opt LPCWSTR lpszAgent,
								 __in DWORD dwAccessType,
								 __in_opt LPCWSTR lpszProxy,
								 __in_opt LPCWSTR lpszProxyBypass,
								 __in DWORD dwFlags 
								 )
{

	CString strProxy;
	strProxy.Format(L"http=http://%s",strHttpProxyInfo);

	HINTERNET TReturn = pInternetOpenW(
		lpszAgent,
		INTERNET_OPEN_TYPE_PROXY,
		strProxy,
		L"<local>",
		dwFlags
		);
	return TReturn;
};

VOID SetProxyForHttp( LPCWSTR pszHttpProxyInfo )
{
	static BOOL bHook = FALSE;
	if( FALSE == bHook )
	{
		bHook = TRUE;

		strHttpProxyInfo = pszHttpProxyInfo;

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach( (PVOID *)&pInternetOpenW,(PVOID)MyInternetOpenW );
		DetourTransactionCommit();

	}
}

BOOL CBaiDuSEOApp::InitInstance()
{
	SetExceptionCatcher();
#if !(defined(DEBUG)|| defined(_DEBUG))
	SetSlient();
#endif

	StartVirtualMouse();
	RegisterBrowserEmulationMode(TRUE);
	BrowserFix();

	CString strProxy;
	CString strKeyWord;
	CString strTargetUrl;

	MyParseCommandLine(GetCommandLineW(),strProxy,strKeyWord,strTargetUrl);
	
	if ( strProxy.GetLength() > 0 )
	{
		SetProxyForHttp( strProxy );
	}

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

	CMainFrame *pMainFrame = new CMainFrame;

	pMainFrame->CreateInstance();

	pMainFrame->m_pView->Navigate(L"about:blank",0,0,0);

	Sleep(100);

	pMainFrame->m_pView->Navigate(L"http://www.baidu.com/",0,0,0);
	//pMainFrame->m_pView->Navigate(L"http://freedev.top/",0,0,0);
	

	m_pMainWnd = pMainFrame;

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

int CBaiDuSEOApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
