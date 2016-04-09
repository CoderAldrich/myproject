#pragma once

#include <afxhtml.h>

#ifdef _WIN32_WCE
#error "Windows CE 不支持 CHtmlView。"
#endif 
#include "IWBCoreControler.h"
#include "IECoreFixWnd.h"

#include <list>
using namespace std;
typedef list<CString> LIST_NEW_WIN_ALLOW,*PLIST_NEW_WIN_ALLOW;
typedef LIST_NEW_WIN_ALLOW::iterator LIST_NEW_WIN_ALLOW_PTR;

class CIECoreView : public CHtmlView,public IWBCoreControler
{
	DECLARE_DYNCREATE(CIECoreView)
public: 
	CIECoreView(); 
	virtual ~CIECoreView();
private:
	static CComPtr<IGlobalInterfaceTable> spGIT;
	CString               m_strLastLocantionUrl;
	CString               m_strLastTitle;
	CIECoreFixWnd         m_wndFixer;
	BOOL                  m_bFixed;
	BOOL                  bCanBack;
	BOOL                  bCanForward;
	BOOL                  bInit;
	HWND                  m_hIEServerWnd;

	static BOOL bInternalHook;
public:
	static CString m_strUserAgent;
	static CString m_strPlatform;
	///////////////////////////
	HANDLE m_hNewWinEvent;
	IWBCoreControler **m_ppNewWBControl;
	LIST_NEW_WIN_ALLOW  m_lstNewWinAllow;

	///////////////////////////
	HANDLE m_hFileDownEvent;
	LPWSTR pszFileUrl;
	UINT   nFileUrlLen;
	//HANDLE m_hDocCompleteEvent;

 public:
 	DWORD m_dwCookie;
public:
	BOOL OnEraseBkgnd(CDC* pDC);
protected:
	int OnCreate(LPCREATESTRUCT lpcs);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()
public:
	HWND GetIEServerWnd();
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnMainDocumentComplete(LPDISPATCH pDisp, VARIANT* URL);
	virtual void NewWindow3( IDispatch **ppDisp,VARIANT_BOOL *Cancel,DWORD dwFlags,BSTR bstrUrlContext,BSTR bstrUrl);
	virtual void OnTitleChange(LPCTSTR lpszText);
	virtual void OnCommandStateChange(long nCommand, BOOL bEnable);

	virtual BOOL CreateControlSite(COleControlContainer* pContainer, 
		COleControlSite** ppSite, UINT nID, REFCLSID clsid);;

	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnStatusTextChange(LPCTSTR lpszText);
	IWebBrowser * GetGlobalWebBrowser(void);
	IWebBrowser2 * GetGlobalWebBrowser2(void);

protected:
	virtual void NavigateComplete2(LPDISPATCH pDisp, VARIANT* URL);
	virtual void BeforeNavigate2(LPDISPATCH pDisp, VARIANT* URL,
		VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData,
		VARIANT* Headers, VARIANT_BOOL* Cancel);
	virtual void DocumentComplete(LPDISPATCH pDisp, VARIANT* URL);
	virtual void NavigateError(LPDISPATCH pDisp, VARIANT* pvURL,
		VARIANT* pvFrame, VARIANT* pvStatusCode, VARIANT_BOOL* pvbCancel);
public:

	virtual HRESULT OnShowContextMenu(DWORD dwID, LPPOINT ppt,
	LPUNKNOWN pcmdtReserved, LPDISPATCH pdispReserved);
	virtual HRESULT OnGetExternal(LPDISPATCH *lppDispatch);
	virtual HRESULT OnGetHostInfo(DOCHOSTUIINFO *pInfo);
	virtual HRESULT OnShowUI(DWORD dwID,
		LPOLEINPLACEACTIVEOBJECT pActiveObject,
		LPOLECOMMANDTARGET pCommandTarget, LPOLEINPLACEFRAME pFrame,
		LPOLEINPLACEUIWINDOW pDoc);
	virtual HRESULT OnHideUI();
	virtual HRESULT OnUpdateUI();
	virtual HRESULT OnEnableModeless(BOOL fEnable);
	virtual HRESULT OnDocWindowActivate(BOOL fActivate);
	virtual HRESULT OnFrameWindowActivate(BOOL fActivate);
	virtual HRESULT OnResizeBorder(LPCRECT prcBorder,
		LPOLEINPLACEUIWINDOW pUIWindow, BOOL fFrameWindow);
	virtual HRESULT OnTranslateAccelerator(LPMSG lpMsg,
		const GUID* pguidCmdGroup, DWORD nCmdID);
	virtual HRESULT OnGetOptionKeyPath(LPOLESTR* pchKey, DWORD dwReserved);
	virtual HRESULT OnGetOverrideKeyPath(LPOLESTR* pchKey, DWORD dwReserved);
	virtual HRESULT OnFilterDataObject(LPDATAOBJECT pDataObject,
		LPDATAOBJECT* ppDataObject);
	virtual HRESULT OnTranslateUrl(DWORD dwTranslate,
		OLECHAR* pchURLIn, OLECHAR** ppchURLOut);
	virtual HRESULT OnGetDropTarget(LPDROPTARGET pDropTarget,
		LPDROPTARGET* ppDropTarget);

	virtual HRESULT OnDownloadFile(LPCWSTR pszFileUrl);
public:
	virtual IWebBrowser2 *GetSafeWebBrowser2();
	virtual IWebBrowser *GetSafeWebBrowser();
	virtual HWND          QueryIEServerWnd();
	virtual unsigned long ControlClose();
	virtual unsigned long ControlQueryWnd(HWND *phWnd);
	virtual unsigned long ControlShowWindow(BOOL bShow);
	virtual unsigned long ControlSetFocus();
	virtual unsigned long ControlMoveWindow(int nLeft,int nTop,int nWidth,int nHeight);
	virtual unsigned long ControlQueryUrl(LPTSTR pszUrl,UINT nLen);
	virtual unsigned long ControlQueryTitle(LPTSTR pszTitle,UINT nLen);
	virtual unsigned long ControlGotoUrl( const wchar_t *pszTargetUrl,const wchar_t *pszReferer);
	virtual unsigned long ControlGoBack( );
	virtual unsigned long ControlGoForward( );
	virtual unsigned long ControlQueryBackForwardStatus(BOOL *pbCanBack,BOOL *pbCanForward);
	virtual unsigned long ControlRefresh( );
	virtual unsigned long ControlStopLoading();
	virtual unsigned long ControlWaitDocumentComplete( DWORD dwTimeOut );
	virtual unsigned long ControlWaitNewWindow( IWBCoreControler **ppWBControl,LPCWSTR *pszAllowUrls,int nAllowCount,DWORD dwTimeOut );
	virtual unsigned long ControlWaitDownloadFile( LPWSTR pszFileUrl, DWORD cchFileUrl ,DWORD dwTimeOut );
	afx_msg void OnPaint();
	//afx_msg void OnTimer(UINT_PTR nIDEvent);
};


