
#pragma once

#include "IE8NaviCmdBar.h"
#include "IE8SearchWnd.h"
#include "IE8NaviBar.h"
#include "IEComboBoxEx.h"

class CIE8NaviBarBGWnd:public CControlBar
{
public:
	CIE8NaviBarBGWnd();
	virtual ~CIE8NaviBarBGWnd();
private:
	CIE8NaviBar m_wndNaviBar;

	CIEComboBoxEx  m_wndAddr;

	CIE8NaviCmdBar m_wndNaviCmdBar;
	CIE8SearchWnd m_wndSearch;
	
	CBitmap     m_bmpNaviBG;

	HDC         m_hMemDC;
	UINT        m_nStatus;
public:
	DECLARE_MESSAGE_MAP()
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
	{

	}

	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnStatusChage(WPARAM wParam,LPARAM lParam);
	void RelayoutChildWindow(void);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//override
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual CSize CalcDynamicLayout(int nLength, DWORD nMode);
public:
	CComboBox * GetAddrBarPtr(void);
	CString GetSearchText();
};