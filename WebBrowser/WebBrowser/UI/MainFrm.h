
// MainFrm.h : CMainFrame 类的接口
//

#pragma once

#include "MenuBar.h"
#include "IE8NaviBGWnd.h"
#include "IE8ReBar.h"
#include "IE8TabBarBGWnd.h"
#include "IE8StatusBar.h"
#include "..\IECoreView.h"


#define WM_ASYNC_URL_CHANGE WM_USER+2000
#define WM_ASYNC_TITLE_CHANGE WM_USER+2001
#define WM_ASYNC_STATUS_TEXT_CHANGE WM_USER+2002
#define WM_ASYNC_FRAME_ADDR_CHANGE WM_USER+2003
#define WM_ASYNC_FRAME_TITLE_CHANGE WM_USER+2004
#define WM_ASYNC_ADD_TAB    WM_USER+2005
#define WM_ASYNC_DEL_TAB    WM_USER+2006
#define WM_ASYNC_SET_FOCUS_TAB WM_USER+2007

class CMainFrame : public CFrameWnd,public IUIControler
{
	
public:
	CMainFrame(BOOL bMutiTab,BOOL bMenuBar,BOOL bToolBar,BOOL bCommandBar);
	CMainFrame();
	virtual ~CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)
protected:

	IUINotifyer      *m_pUINotifyer;

	//公共元素
	CIE8ReBar         m_wndRebar;
	CIE8StatusBar     m_wndStatusBar;
	C58MenuBar        m_IEMenuBar;

	//IE8
	CIE8NaviBarBGWnd  m_wndNaviBar;	
	CIE8TabBarBGWnd   m_wndTabBar;

	//
	CComboBox        *m_pCurAddr;

	//
	BOOL bCanBack;
	BOOL bCanForward;

	BOOL bFirstErase;
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	
// 实现

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected: 
	void OnUICommand(UINT nCommandID);
	void OnDoNothing() {
		int a=0;
	};
// 生成的消息映射函数
protected:
	afx_msg int     OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnTabSelChange( WPARAM wParam,LPARAM lParam );
	afx_msg LRESULT OnKeyTabChange(WPARAM wParam,LPARAM lParam );
	afx_msg LRESULT OnTabClose( WPARAM wParam,LPARAM lParam );
	afx_msg LRESULT OnDragTab( WPARAM wParam,LPARAM lParam );
	afx_msg void    OnUpdateBackButton(CCmdUI *);
	afx_msg void    OnUpdateForwardButton(CCmdUI *);
	afx_msg void    OnClose();
	
	DECLARE_MESSAGE_MAP()

protected:
	//异步调用封装 消息
	LRESULT OnAsyncUrlChange(WPARAM wParam,LPARAM lParam);
	LRESULT OnAsyncTitleChange(WPARAM wParam,LPARAM lParam);
	LRESULT OnAsyncStatusTextChange(WPARAM wParam,LPARAM lParam);
	LRESULT OnAsyncFrameAddrChange(WPARAM wParam,LPARAM lParam);
	LRESULT OnAsyncFrameTitleChange(WPARAM wParam,LPARAM lParam);
	LRESULT OnAsyncAddTab(WPARAM wParam,LPARAM lParam);
	LRESULT OnAsyncDelTab(WPARAM wParam,LPARAM lParam);
	LRESULT OnAsyncSetFocusTab(WPARAM wParam,LPARAM lParam);
public:
	void SetActiveView(CView* pViewNew, BOOL bNotify = TRUE);
	virtual CDocument* GetActiveDocument();
	CView* GetActiveView() const;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	BOOL OnEraseBkgnd(CDC* pDC);
//实现接口
public:
	bool                  SetNofiyerPoint( IUINotifyer *pNotifyer );
	bool                  GetNofiyerPoint( IUINotifyer **ppNotifyer );
	virtual unsigned long ControlQueryQueryMainFrame(HWND *phMainFrame) ;
	virtual unsigned long ControlQueryClientRect(LPRECT pRect);

	virtual unsigned long ControlPageUrlChange(PAGEID nPageID,const wchar_t * pszNewUrl);
	virtual unsigned long ControlPageTitleChange(PAGEID nPageID,const wchar_t * pszNewTitle);
	virtual unsigned long ControlFrameStatusChage(const wchar_t * pszNewStatus);
	virtual unsigned long ControlFrameAddrChange(const wchar_t * pszNewAddr);

	virtual unsigned long ControlFrameTitleChange(const wchar_t * pszNewTitle);

	virtual unsigned long ControlBackForward(BOOL bCanBack,BOOL bCanForward);

	virtual unsigned long ControlAddTab(PAGEID nPageID, LPCTSTR pszUrl = NULL,LPCTSTR pszTitle = NULL);
	virtual unsigned long ControlDelTab( PAGEID nPageID );
	virtual unsigned long ControlSetFocusTab( PAGEID nPageID );
	

	afx_msg BOOL OnNcActivate(BOOL bActive);
};


