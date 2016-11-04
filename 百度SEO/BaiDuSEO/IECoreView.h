#pragma once

#include <afxhtml.h>

#ifdef _WIN32_WCE
#error "Windows CE 不支持 CHtmlView。"
#endif 


#include "IECoreFixWnd.h"

class CIECoreView : public CHtmlView
{
	DECLARE_DYNCREATE(CIECoreView)
public: 
	CIECoreView(); 
	virtual ~CIECoreView();
private:
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

	afx_msg void OnPaint();
	//afx_msg void OnTimer(UINT_PTR nIDEvent);
};


