// WebBrowser.cpp : ����Ӧ�ó��������Ϊ��
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

#include ".\���������\BrowserProtect.h"
// CWebBrowserApp ����
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

// Ψһ��һ�� CWebBrowserApp ����
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
	// ������׼������DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	CString str;
	LPWSTR *szArglist = NULL;  //�������ַ���ָ��,szArglist[i]�����i���ַ�������
	int nArgs = 0; //nArgs�����в����ĸ���  
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);//�����в�����������

	if(szArglist!= NULL)   
	{
		USES_CONVERSION; //���ͷ�ļ�����ĺ�
		for (int i=0;i<nArgs;i++)
		{
			str=W2CT(szArglist[i]);//LPWSTRת��ΪLPCSTR
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
	OutputDebugStringW(L"��ȡ��ҳ "+strHomePage+L"\n");
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
