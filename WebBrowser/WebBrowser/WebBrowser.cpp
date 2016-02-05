// WebBrowser.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include <windows.h>
#include "WebBrowser.h"
#include ".\UI\MainFrm.h"
#include "INotifyerInstance.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "SimpleFrameWnd.h"

#include "NoParentThread.h"


#include "BrowserHelpFun.h"

UINT nHtmlMsg = ::RegisterWindowMessage(_T("WM_HTML_GETOBJECT"));

BEGIN_MESSAGE_MAP(CWebBrowserApp, CWinApp)
END_MESSAGE_MAP()

#include ".\浏览器保护\BrowserProtect.h"
// CWebBrowserApp 构造
CWebBrowserApp::CWebBrowserApp()
{
	m_dwFrameCount = 0;
	m_nBaseStyle = 8;
	m_bMutiTab = TRUE;
	m_bMenuBar = TRUE;
	m_bToolBar = TRUE;
	m_bCommandBar = TRUE;
	strHomePage = L"about:blank";
	//StartProtect(L"sdf");
}

// 唯一的一个 CWebBrowserApp 对象
CWebBrowserApp theApp;

BOOL CWebBrowserApp::InitInstance()
{
	::LoadLibrary(L"DebugPrivate.dll");

	BrowserFix();
	RegisterBrowserEmulationMode(FALSE);

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();
    afxAmbientActCtx = FALSE;

      if (!AfxOleInit())
      {
      	AfxMessageBox(IDP_OLE_INIT_FAILED);
      	return FALSE;
      }

	AfxEnableControlContainer();
	// 分析标准外壳命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	CString str;
	LPWSTR *szArglist = NULL;  //命令行字符串指针,szArglist[i]代表第i个字符串变量
	int nArgs = 0; //nArgs命令行参数的个数  
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);//命令行参数解析函数

	if(szArglist!= NULL)   
	{
		USES_CONVERSION; //相关头文件定义的宏
		for (int i=0;i<nArgs;i++)
		{
			str=W2CT(szArglist[i]);//LPWSTR转换为LPCSTR
			if (str.GetAt(0) == L'-')
			{
				CString strParamName;
				strParamName = str;
				if (strParamName.CompareNoCase(L"-forceurl") == 0 && i+1 < nArgs)
				{
					strHomePage = szArglist[i+1];
					i++;
				}
			}
			
		}
	} 
	LocalFree(szArglist);

#ifdef DEBUG
	//strHomePage=L"https://123.sogou.com/";
	OutputDebugStringW(L"获取主页 "+strHomePage+L"\n");
#endif

// 	m_bMutiTab = FALSE;
// 	m_nBaseStyle = 9;
	AfxBeginThread(RUNTIME_CLASS(CNoParentThread));
	while ( m_pMainWnd == NULL )
    {
        MSG msg;
        if (::PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            if ((msg.message == WM_QUIT) || (msg.message == WM_CLOSE) || (msg.message == WM_DESTROY))
            {
                Sleep(100);
            }
            else
            {
                TranslateMessage (&msg) ;
                DispatchMessage (&msg) ;
            }
        }
        else
        {
            Sleep(100);
        }
    }

	NewBrowserInstance(NULL,NULL,strHomePage);

	return TRUE;
}


int CWebBrowserApp::Run()
{
    while ( m_dwFrameCount > 0 )
    {
        MSG msg;
        if (::PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            if ((msg.message == WM_QUIT) || (msg.message == WM_CLOSE) || (msg.message == WM_DESTROY))
            {
                Sleep(100);
            }
            else
            {
                TranslateMessage (&msg) ;
                DispatchMessage (&msg) ;
            }
        }
        else
        {
            Sleep(100);
        }
    }

    //m_pMainWnd->PostMessage(WM_CLOSE,0,0);

    return 0;
}

IGlobalInterfaceTable *CWebBrowserApp::GetGITPtr()
{
	if (spGIT == NULL)
	{
		HRESULT hr = spGIT.CoCreateInstance(CLSID_StdGlobalInterfaceTable);
	}

	return spGIT;

}
void CWebBrowserApp::AddFrameThread(HANDLE hFrameThread)
{
	
}
void CWebBrowserApp::DelFrameThread(HANDLE hFrameThread)
{
	
}
void CWebBrowserApp::IncreaseFrameCount(void)
{
	InterlockedIncrement(&m_dwFrameCount);
}

void CWebBrowserApp::DecreaseFrameCount(void)
{
 	InterlockedDecrement(&m_dwFrameCount);
}

BOOL CWebBrowserApp::ExitInstance(void)
{
    CoUninitialize();
    OleUninitialize();
	return CWinApp::ExitInstance();
}
