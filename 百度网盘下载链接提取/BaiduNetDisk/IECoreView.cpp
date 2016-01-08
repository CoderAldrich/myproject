// IECoreView.cpp : 实现文件
//

#include "stdafx.h"
#include "IECoreView.h"
#include <ExDispid.h>
#include <strsafe.h>
#include "IIEOleClientSite.h"
#include "MainFrm.h"
#include "SAStatusLog.h"
#include "PublicFun.h"

extern CSAStatusLog g_Loger;

BEGIN_EVENTSINK_MAP(CIECoreView,  CHtmlView)
ON_EVENT(CIECoreView,  AFX_IDW_PANE_FIRST ,DISPID_NEWWINDOW3,NewWindow3,VTS_PDISPATCH  VTS_PBOOL  VTS_I4  VTS_BSTR  VTS_BSTR)
END_EVENTSINK_MAP()
// CIECoreView

IMPLEMENT_DYNCREATE(CIECoreView, CHtmlView)

CComPtr<IGlobalInterfaceTable> CIECoreView::spGIT = NULL;

static UINT nHtmlMsg = ::RegisterWindowMessage( L"WM_HTML_GETOBJECT" );

WNDPROC oldIeProc = 0;
LRESULT CALLBACK IE_WindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	if(
		nHtmlMsg == uMsg
		|| WM_GETOBJECT == uMsg
		)
	{
		return 0;
	}
	return ::CallWindowProc((WNDPROC)oldIeProc,hwnd,uMsg,wParam,lParam);
}


//BOOL g_bInjectMatrix = FALSE;
BOOL g_bClickMatrix = FALSE;
extern CString g_strJsUrl;

CIECoreView::CIECoreView()
{
	bCanBack = FALSE;
	bCanForward = FALSE;
	bInit = FALSE;
	m_bFixed = FALSE;
	m_dwCookie = 0;
	m_hIEServerWnd = NULL;
	if ( spGIT == NULL )
	{
		spGIT.CoCreateInstance(CLSID_StdGlobalInterfaceTable);
	}
}

CIECoreView::~CIECoreView()
{
	spGIT->RevokeInterfaceFromGlobal( m_dwCookie );
	
	int a=0;
}

void CIECoreView::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CIECoreView, CHtmlView)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CIECoreView 诊断

#ifdef _DEBUG
void CIECoreView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CIECoreView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}
#endif //_DEBUG

HWND CIECoreView::GetIEServerWnd()
{
	return m_hIEServerWnd;
}
int CIECoreView::OnCreate(LPCREATESTRUCT lpcs)
{
	int nRes = CHtmlView::OnCreate(lpcs);

	return nRes;
}
BOOL CIECoreView::OnEraseBkgnd(CDC* pDC)
{
	return true;
}

BOOL CIECoreView::CreateControlSite(COleControlContainer* pContainer, 
					   COleControlSite** ppSite, UINT nID, REFCLSID clsid)
{
 	if(ppSite == NULL)
 	{
 		ASSERT(FALSE);
 		return FALSE;
 	}
 	IIEOleControlSite* m_pBrowserSite =  new IIEOleControlSite (pContainer,this);
 	if (!m_pBrowserSite)
 		return FALSE;
 	*ppSite = m_pBrowserSite;

	return TRUE;
}

// CIECoreView 消息处理程序

void CIECoreView::NewWindow3( IDispatch **ppDisp,VARIANT_BOOL *Cancel,DWORD dwFlags,BSTR bstrUrlContext,BSTR bstrUrl)
{
	if (!ppDisp)
		return;
	*Cancel = VARIANT_TRUE;
} 


void CIECoreView::OnTitleChange(LPCTSTR lpszText)
{
    if(CString(lpszText)==_T("about:blank"))
    {
        return ;
    }
    
	m_strLastTitle = lpszText;

	CHtmlView::OnTitleChange(lpszText);
}

void CIECoreView::OnCommandStateChange(long nCommand, BOOL bEnable)
{
	BOOL bChanged = TRUE;
	switch (nCommand)
	{
	case CSC_NAVIGATEBACK:
		{
			if( bCanBack != bEnable )
			{
				int a=0;
			}
			bCanBack = bEnable;
		}
		break;
	case CSC_NAVIGATEFORWARD:
		{
			if( bCanForward != bEnable )
			{
				int a=0;
			}
			bCanForward = bEnable;
		}
		break;
	default:
		{
			bChanged = FALSE;
		}
	}
	CHtmlView::OnCommandStateChange(nCommand, bEnable);
}

void CIECoreView::NavigateComplete2(LPDISPATCH pDisp, VARIANT* URL)
{
	if(pDisp == GetApplication() /* 仅处理最顶层Frame的事件 */)
	{
		if(m_bFixed == FALSE)
		{
			m_bFixed = TRUE;

			m_pBrowserApp->put_Silent(VARIANT_TRUE);
			m_pBrowserApp->put_RegisterAsBrowser(VARIANT_FALSE);

			HWND hEmbed=NULL;
			HWND hDoc=NULL;
			if(m_hWnd)
			{
				hEmbed=::FindWindowExW(m_hWnd,NULL,TEXT("Shell Embedding"),NULL);
				if(hEmbed!=NULL)
				{
					hDoc=::FindWindowExW(hEmbed,NULL,TEXT("Shell DocObject View"),NULL);
					if(hDoc!=NULL)
					{
						m_hIEServerWnd=::FindWindowExW(hDoc,NULL,TEXT("Internet Explorer_Server"),NULL);
					}
				}
			}
			if(m_hIEServerWnd!=NULL )
			{
				if( NULL == oldIeProc)
				{
					oldIeProc=(WNDPROC)GetClassLong(m_hIEServerWnd,GCL_WNDPROC);
					if(oldIeProc!=NULL)
					{
						::SetWindowLong(m_hIEServerWnd,GWL_WNDPROC,(LONG)IE_WindowProc);//修改当前窗口
						::SetClassLong(m_hIEServerWnd,GCL_WNDPROC,(LONG)IE_WindowProc);//修改所有的窗口过程
					}
				}

				m_wndFixer.SubclassWindow(m_hIEServerWnd);

			}
		}

		OnMainDocumentComplete(pDisp, URL);
	}
}

void CIECoreView::BeforeNavigate2(LPDISPATCH pDisp, VARIANT* URL,
							 VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData,
							 VARIANT* Headers, VARIANT_BOOL* Cancel)
{

	if (GetApplication() == pDisp)
	{
	}
}

void CIECoreView::OnMainDocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
{
	
}

void CIECoreView::DocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
{
	if (GetApplication() == pDisp)
	{
		OnMainDocumentComplete(pDisp,URL);
	}
}
void CIECoreView::NavigateError(LPDISPATCH pDisp, VARIANT* pvURL,
						   VARIANT* pvFrame, VARIANT* pvStatusCode, VARIANT_BOOL* pvbCancel)
{
	if(pDisp == GetApplication()/* 仅处理最顶层Frame的事件 */)
	{
		
	}
}

unsigned long CIECoreView::ControlClose()
{
	::PostMessage(m_hWnd,WM_CLOSE,0,0);
	return 0;
}
unsigned long CIECoreView::ControlQueryWnd(HWND *phWnd)
{
	if(phWnd)
	{
		*phWnd = m_hWnd;
	}
	return 0;
}
unsigned long CIECoreView::ControlQueryUrl(LPTSTR pszUrl,UINT nLen)
{
	wcscpy_s(pszUrl,nLen,m_strLastLocantionUrl.GetBuffer());
	return 0;
}
unsigned long CIECoreView::ControlQueryTitle(LPTSTR pszTitle,UINT nLen)
{
	wcscpy_s(pszTitle,nLen,m_strLastTitle.GetBuffer());
	return 0;
}
unsigned long CIECoreView::ControlGotoUrl( const wchar_t *pszTargetUrl )
{
	IWebBrowser *pWb = GetGlobalWebBrowser();
	CString strUrl;
	strUrl = pszTargetUrl;
	CComVariant vt;
	if(pWb)
	{
	    pWb->Navigate(strUrl.AllocSysString(),&vt,&vt,&vt,&vt);
    }
	return 0;
}
unsigned long CIECoreView::ControlGoBack( )
{
	IWebBrowser *pWb = GetGlobalWebBrowser();
	if(pWb)
	{
	    pWb->GoBack();
    }
	return 0;
}
unsigned long CIECoreView::ControlGoForward( )
{
	IWebBrowser *pWb = GetGlobalWebBrowser();
	if(pWb)
	{
	    pWb->GoForward();
    }
	return 0;
}
unsigned long CIECoreView::ControlQueryBackForwardStatus(BOOL *pbCanBack,BOOL *pbCanForward)
{
	if (pbCanBack)
	{
		*pbCanBack = bCanBack;
	}
	if (pbCanForward)
	{
		*pbCanForward = bCanForward;
	}
	return 0;
}

unsigned long CIECoreView::ControlRefresh( )
{
	IWebBrowser *pWb = GetGlobalWebBrowser();
	if(pWb)
	{
	    pWb->Refresh();
    }
	return 0;
}
unsigned long CIECoreView::ControlStopLoading()
{
	IWebBrowser *pWb = GetGlobalWebBrowser();
	if(pWb)
	{
	    pWb->Stop();
    }
	return 0;
}
unsigned long CIECoreView::ControlEditCut()
{
	IWebBrowser2 *pWb2 = GetGlobalWebBrowser2();
	pWb2->ExecWB(OLECMDID_OPEN,OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	return 0;
}
unsigned long CIECoreView::ControlEditCopy()
{
	IWebBrowser2 *pWb2 = GetGlobalWebBrowser2();
	//pWb2->ExecWB(IDM_COPY, NULL, NULL);
	return 0;
}
unsigned long CIECoreView::ControlEditPaste()
{
	IWebBrowser2 *pWb2 = GetGlobalWebBrowser2();
	//pWb2->ExecWB(IDM_PASTE, NULL, NULL);
	return 0;
}
unsigned long CIECoreView::ControlEditSelectAll()
{
	IWebBrowser2 *pWb2 = GetGlobalWebBrowser2();
	//pWb2->ExecWB(IDM_SELECTALL, NULL, NULL);
	return 0;
}
unsigned long CIECoreView::ControlEditFind()
{
	IWebBrowser2 *pWb2 = GetGlobalWebBrowser2();
	//pWb2->ExecFormsCommand(IDM_FIND, NULL, NULL);
	return 0;
}
unsigned long CIECoreView::ControlQueryWKBridge(PVOID *pWKBridge)
{
    if(pWKBridge)
    {
        *pWKBridge = NULL;
    }
    return 0;
}
void CIECoreView::OnSize(UINT nType, int cx, int cy)
{
	CHtmlView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
}

void CIECoreView::OnStatusTextChange(LPCTSTR lpszText)
{
}

IWebBrowser * CIECoreView::GetGlobalWebBrowser(void)
{
	if ( m_dwCookie == 0 )
	{
		spGIT->RegisterInterfaceInGlobal(GetApplication(),IID_IDispatch,&m_dwCookie);
	}

 	IDispatch *pDisp;
 	spGIT->GetInterfaceFromGlobal(m_dwCookie,IID_IDispatch,(void **)&pDisp);
 	IWebBrowser *pWb = NULL;
 	pDisp->QueryInterface(IID_IWebBrowser,(void **)&pWb);
 	return pWb;
}
IWebBrowser2 * CIECoreView::GetGlobalWebBrowser2(void)
{
	//return m_pBrowserApp;

 	IWebBrowser2 *pWb2 = NULL;
 	IWebBrowser *pWb = GetGlobalWebBrowser();
 	if(pWb)
 	{
 		pWb->QueryInterface(IID_IWebBrowser2,(void **)&pWb2);
 	}
 
 	return pWb2;
}
void CIECoreView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

}

HRESULT CIECoreView::OnShowContextMenu(DWORD dwID, LPPOINT ppt,
								  LPUNKNOWN pcmdtReserved, LPDISPATCH pdispReserved)
{
	return E_NOTIMPL;
}
HRESULT CIECoreView::OnGetExternal(LPDISPATCH *lppDispatch)
{
	//不应该返回E_NOIMPLE 否则腾讯视频看不了
	return S_OK;
}
HRESULT CIECoreView::OnGetHostInfo(DOCHOSTUIINFO *pInfo)
{
	pInfo->cbSize = sizeof(DOCHOSTUIINFO);
	pInfo-> dwFlags= DOCHOSTUIFLAG_NO3DBORDER|DOCHOSTUIFLAG_THEME;
	pInfo-> dwDoubleClick= DOCHOSTUIDBLCLK_DEFAULT;
	return S_OK;
}
HRESULT CIECoreView::OnShowUI(DWORD dwID,
						 LPOLEINPLACEACTIVEOBJECT pActiveObject,
						 LPOLECOMMANDTARGET pCommandTarget, LPOLEINPLACEFRAME pFrame,
						 LPOLEINPLACEUIWINDOW pDoc)
{
	return E_NOTIMPL;
}
HRESULT CIECoreView::OnHideUI()
{
	return E_NOTIMPL;
}
HRESULT CIECoreView::OnUpdateUI()
{
	return E_NOTIMPL;
}
HRESULT CIECoreView::OnEnableModeless(BOOL fEnable)
{
	return E_NOTIMPL;
}
HRESULT CIECoreView::OnDocWindowActivate(BOOL fActivate)
{
	return E_NOTIMPL;
}
HRESULT CIECoreView::OnFrameWindowActivate(BOOL fActivate)
{
	return E_NOTIMPL;
}
HRESULT CIECoreView::OnResizeBorder(LPCRECT prcBorder,
							   LPOLEINPLACEUIWINDOW pUIWindow, BOOL fFrameWindow)
{
	return E_NOTIMPL;
}
HRESULT CIECoreView::OnTranslateAccelerator(LPMSG lpMsg,
									   const GUID* pguidCmdGroup, DWORD nCmdID)
{
	return E_NOTIMPL;
}
HRESULT CIECoreView::OnGetOptionKeyPath(LPOLESTR* pchKey, DWORD dwReserved)
{
	return E_NOTIMPL;
}

HRESULT CIECoreView::OnGetOverrideKeyPath(LPOLESTR* pchKey, DWORD dwReserved)
{
	return E_NOTIMPL;
}
HRESULT CIECoreView::OnFilterDataObject(LPDATAOBJECT pDataObject,
								   LPDATAOBJECT* ppDataObject)
{
	return E_NOTIMPL;
}
HRESULT CIECoreView::OnTranslateUrl(DWORD dwTranslate,
							   OLECHAR* pchURLIn, OLECHAR** ppchURLOut)
{
	return E_NOTIMPL;
}
HRESULT CIECoreView::OnGetDropTarget(LPDROPTARGET pDropTarget,
								LPDROPTARGET* ppDropTarget)
{
	return E_NOTIMPL;
}

HRESULT CIECoreView::OnDownloadFile( BSTR bstrFileUrl )
{
	return E_NOTIMPL;
}


BOOL CIECoreView::PreTranslateMessage(MSG* pMsg)
{
    return CHtmlView::PreTranslateMessage(pMsg);
}

