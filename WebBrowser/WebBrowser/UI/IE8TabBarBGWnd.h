#pragma once


// CIE8TabBarBGWnd
#include "IE8TabBar.h"
#include "EasyToolBar.h"
class CIE8TabBarBGWnd : public CControlBar
{
	DECLARE_DYNAMIC(CIE8TabBarBGWnd)
private:
	CIETabBar m_wndTabBar;
	CEasyToolBar   m_wndFavBtn;
	CEasyToolBar   m_wndCmdBar;
	CPen      m_Pen;
	BOOL         m_bBaseTheme;
public:
	CIE8TabBarBGWnd();
	virtual ~CIE8TabBarBGWnd();
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	//override
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual CSize CalcDynamicLayout(int nLength, DWORD nMode);
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	CIETabBar *GetTabCtrl();
};