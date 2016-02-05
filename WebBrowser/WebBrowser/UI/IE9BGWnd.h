#pragma once


// CIE9BGWnd
#include "IE8TabBar.h"
#include "EasyToolBar.h"
#include "IE9AddrBar.h"
class CIE9BGWnd : public CControlBar
{
	DECLARE_DYNAMIC(CIE9BGWnd)
public:
	CIETabBar m_wndTabBar;
	CEasyToolBar m_wndNaviBar;
	CIE9AddrBar m_wndAddr;
	CEasyToolBar m_wndCmdBar;
	BOOL   m_bHideTabBar;
public:
	CIE9BGWnd();
	virtual ~CIE9BGWnd();
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	CSize CalcDynamicLayout(int nLength, DWORD nMode);
	BOOL OnEraseBkgnd(CDC* pDC);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	CComboBox *GetAddrBarPtr();
	CIETabBar *GetTabBarPtr();
	VOID       HideTabBar();
};


