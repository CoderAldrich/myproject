
// ShuaClient.cpp : ����Ӧ�ó��������Ϊ��
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

#include "AutoBrowser.h"
#include "BaiDuSEOView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CBaiDuSEOApp, CWinApp)

END_MESSAGE_MAP()


// CShuaClientApp ����

CBaiDuSEOApp::CBaiDuSEOApp()
{

}

// Ψһ��һ�� CShuaClientApp ����

CBaiDuSEOApp theApp;

BOOL SetSlient();
BOOL SetExceptionCatcher();
BOOL g_bPhoneMode = TRUE;


//���������в���
VOID MyParseCommandLine(
					  LPCWSTR pszRunCmd,
					  CString &strProxy,
					  CString &strKeyWord,
					  CString &strTargetUrl,
					  CString &strUserAgent,
					  CString &strPlatform
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
			if ( ( wChar == L'-' && (chPreChar == 0 || chPreChar == L' ') ) || i+1==nCmdLen/*���һ���ַ�*/)
			{
				bInParamName = TRUE;
				bInParamValue = FALSE;

				if ( i+ 1== nCmdLen )//��������һ���ַ�
				{
					strTempParamValue+=wChar;
				}

				strTempParamValue.Trim();

				//�˴����������в���
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

				if ( strTempParamName.CompareNoCase(L"-useragent") == 0 )
				{
					strUserAgent = strTempParamValue;
				}

				if ( strTempParamName.CompareNoCase(L"-platform") == 0 )
				{
					strPlatform = strTempParamValue;
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
CString g_strSearchKeyWord;
CString g_strTargetUrl;
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

DWORD WINAPI WorkThread( PVOID pParam )
{
	BOOL bRight = FALSE;

	CBaiDuSEOView *pBaiduView = (CBaiDuSEOView *)pParam;

	pBaiduView->Navigate(L"about:blank",0,0,0);
	Sleep(100);
	pBaiduView->Navigate(L"http://www.baidu.com/",0,0,0);

	//pMainFrame->m_pView->Navigate(L"http://freedev.top/",0,0,0);

	if(pBaiduView->WaitDocumentComplete(20000))
	{
		Sleep(5000);
		CAutoBrowser AutoBrowser(pBaiduView->GetGlobalWebBrowser2(),pBaiduView->GetIEServerWnd());

		CElementInformation InputElem;
		InputElem.SetTagName(L"input");
		InputElem.AddElementAttribute(L"name",L"wd",TRUE);
		AutoBrowser.ClickFirstMatchWebPageElement(&InputElem);
		Sleep(1000);

		AutoBrowser.InputText(g_strSearchKeyWord);
		Sleep(1000);

		{
			CElementInformation ElemInfo;
			ElemInfo.SetTagName(L"input");
			ElemInfo.AddElementAttribute(L"value",L"�ٶ�һ��",TRUE);

			AutoBrowser.ClickFirstMatchWebPageElement(&ElemInfo);
		}


		Sleep(5000);

		BOOL bNextFound = FALSE;
		int  nRefreshCount = 0;

		for( int i=0;i<100;i++ )
		{
			pBaiduView->WaitDocumentComplete(20000);
			Sleep(5000);


			CElementInformation ElemInfo;
			ElemInfo.SetTagName(L"a");
			ElemInfo.SetTextName(g_strTargetUrl,FALSE);
			ElemInfo.AddElementAttribute(L"href",L"http",FALSE);

			CElemRectList lstElems;
			AutoBrowser.GetAllMatchElemRect(&lstElems,&ElemInfo);
			if (lstElems.GetElemRectCount() > 0)
			{
				ELEM_RECT rcElem;
				lstElems.GetElemRectByIndex(0,&rcElem);

				CComQIPtr<IHTMLElement> pElem;
				if( S_OK == rcElem.pElem->QueryInterface(IID_IHTMLElement,(void **)&pElem) && pElem )
				{

					CString strTargetUrl;
					CComVariant vtAttrValue;
					HRESULT hr =pElem->getAttribute(CString(L"href").AllocSysString(),2,&vtAttrValue);

					if (S_OK == hr && vtAttrValue.vt == VT_BSTR && CString(vtAttrValue.bstrVal).GetLength() > 0)
					{
						strTargetUrl = vtAttrValue.bstrVal;
					}

					CElementInformation ElemInfo;
					ElemInfo.SetTagName(L"a");
					ElemInfo.AddElementAttribute(L"href",strTargetUrl,FALSE);
					CElementInformation *pParentElemInfo = ElemInfo.CreateParentInfo();
					if (pParentElemInfo)
					{
						pParentElemInfo->SetTagName(L"h3");
						pParentElemInfo->AddElementAttribute(L"class",L"t",TRUE);
					}

					AutoBrowser.ClickFirstMatchWebPageElement(&ElemInfo);

					bRight = TRUE;
				}

				break;
			}
	

			BOOL bNextClick = FALSE;

			for( int n=0;n<10;n++ )
			{
				CElementInformation ElemInfo;
				ElemInfo.SetTagName(L"a");
				ElemInfo.SetTextName(L"��һҳ",FALSE);
				ElemInfo.AddElementAttribute(L"href",L"/s",FALSE);

				if( AutoBrowser.ClickFirstMatchWebPageElement(&ElemInfo) )
				{
					bNextClick = TRUE;
					bNextFound = TRUE;
					nRefreshCount = 0;
					break;
				}

				Sleep(1000);
			}

			if ( FALSE == bNextClick )
			{
				if ( bNextFound && nRefreshCount < 3 )
				{
					nRefreshCount++;
					pBaiduView->Refresh();
				}
				else
				{
					break;
				}
				
			}

		}

	}

	if (bRight)
	{
		Sleep(CAutoBrowser::GetRandValue(15000,60000));
	}
	

	ExitProcess(0);

	return 0;
}

BOOL CBaiDuSEOApp::InitInstance()
{
	SetExceptionCatcher();
// #if !(defined(DEBUG)|| defined(_DEBUG))
// 	SetSlient();
// #endif

	StartVirtualMouse();
	RegisterBrowserEmulationMode(TRUE);
	BrowserFix();

	//��ֹ��ȡ����
	LockSetForegroundWindow( LSFW_LOCK	);

	CString strProxy;
	CString strKeyWord;
	CString strTargetUrl;
	CString strUseAgent;
	CString strPlatform;

	MyParseCommandLine(GetCommandLineW(),strProxy,strKeyWord,strTargetUrl,strUseAgent,strPlatform);
	
	if ( strProxy.GetLength() > 0 )
	{
		SetProxyForHttp( strProxy );
	}

	if ( strKeyWord.GetLength() == 0 || strTargetUrl.GetLength() == 0 )
	{
		return FALSE;
	}

	if ( strUseAgent.GetLength() > 0 )
	{
		CIECoreView::m_strUserAgent = strUseAgent;
	}

	if ( strPlatform.GetLength() > 0 )
	{
		CIECoreView::m_strPlatform = strPlatform;
	}

	g_strSearchKeyWord = strKeyWord;
	g_strTargetUrl = strTargetUrl;


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

	m_pMainWnd = pMainFrame;

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	::CreateThread(NULL,0,WorkThread,pMainFrame->m_pView,0,NULL);


	return TRUE;
}

int CBaiDuSEOApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
