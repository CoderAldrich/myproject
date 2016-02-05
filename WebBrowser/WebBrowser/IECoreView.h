#pragma once

#ifdef _WIN32_WCE
#error "Windows CE 不支持 CHtmlView。"
#endif 

// CIECoreView Html 视图
#include "IWBCoreNotifyer.h"
#include "IWBCoreControler.h"
#include "IECoreFixWnd.h"

typedef enum
{	
	CC_ERROR=0,
	CC_GO_BACK = 1,
	CC_GO_FORWARD = 2,
	CC_REFRESH = 3,
	CC_STOP = 4,
	CC_EDIT_CUT = 5,
	CC_EDIT_COPY = 6,
	CC_EDIT_PASTE = 7,
	CC_EDIT_SELALL = 8,
	CC_ZOOM = 9

}WB_CONTROL_CODE;

#define WM_ASYNC_GOTO_URL WM_USER+3000
#define WM_ASYNC_SIMPLE_CONTROL WM_USER+3001
#define WM_ASYNC_SHOW_WINDOW WM_USER+3002
#define WM_ASYNC_MOVE_WINDOW WM_USER+3003
#define WM_ASYNC_SET_FOCUS WM_USER+3004
#define WM_ASYNC_ZOOM       WM_USER+3005

class CIECoreView : public CHtmlView,public IWBCoreControler
{
	DECLARE_DYNCREATE(CIECoreView)
public:
	CIECoreView(IWBCoreNotifyer *pNotifyer);           // 动态创建所使用的受保护的构造函数
	CIECoreView();   
	virtual ~CIECoreView();
private:
	IWBCoreNotifyer      *m_pNotifyer;
	PAGEID                m_PageID;
	CString               m_strLastLocantionUrl;
	CString               m_strLastTitle;
	CIECoreFixWnd         m_wndFixer;
	BOOL                  m_bFixed;
	HWND   m_hIEServer;
	BOOL m_bCanBack;
	BOOL m_bCanForward;
	BOOL bInit;
	INT  m_nCurZoom;
 public:
 	DWORD m_dwCookie;
public:

	BOOL OnEraseBkgnd(CDC* pDC);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);

	void NewWindow3( IDispatch **ppDisp,VARIANT_BOOL *Cancel,DWORD dwFlags,BSTR bstrUrlContext,BSTR bstrUrl);
	virtual void OnTitleChange(LPCTSTR lpszText);
	virtual void OnCommandStateChange(long nCommand, BOOL bEnable);

 	BOOL CreateControlSite(COleControlContainer* pContainer, 
 		COleControlSite** ppSite, UINT nID, REFCLSID clsid);

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

protected:
	//异步控制 处理函数
	LRESULT OnAsyncGotoUrl(WPARAM wParam,LPARAM lParam);
	LRESULT OnAsyncSimpleControl(WPARAM wParam,LPARAM lParam);
	LRESULT OnAsyncShowWindow(WPARAM wParam,LPARAM lParam);
	LRESULT OnAsyncMoveWindow(WPARAM wParam,LPARAM lParam);
	LRESULT OnAsyncSetFocus(WPARAM wParam,LPARAM lParam);
	LRESULT OnAsyncZoom(WPARAM wParam,LPARAM lParam);
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
public:
	virtual unsigned long SetNotifyPtr( IWBCoreNotifyer *pNotifyer );
	virtual unsigned long GetNotifyPtr( IWBCoreNotifyer **ppNotifyer );
	virtual unsigned long SetPageID( PAGEID nPageID );
	virtual unsigned long GetPageID( PAGEID *pPageID);
	virtual unsigned long ControlClose();
	virtual unsigned long ControlQueryWnd(HWND *phWnd);
	virtual unsigned long ControlShowWindow(BOOL bShow);
	virtual unsigned long ControlSetFocus();
	virtual unsigned long ControlMoveWindow(int nLeft,int nTop,int nWidth,int nHeight);
	virtual unsigned long ControlQueryUrl(LPTSTR pszUrl,UINT nLen);
	virtual unsigned long ControlQueryTitle(LPTSTR pszTitle,UINT nLen);
	virtual unsigned long ControlGotoUrl( const wchar_t *pszTargetUrl );
	virtual unsigned long ControlGoBack( );
	virtual unsigned long ControlGoForward( );
	virtual unsigned long ControlQueryBackForwardStatus(BOOL *pbCanBack,BOOL *pbCanForward);
	virtual unsigned long ControlRefresh( );
	virtual unsigned long ControlStopLoading();
	virtual unsigned long ControlEditCut();
	virtual unsigned long ControlEditCopy();
	virtual unsigned long ControlEditPaste();
	virtual unsigned long ControlEditSelectAll();
	virtual unsigned long ControlEditFind();
	

	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};


