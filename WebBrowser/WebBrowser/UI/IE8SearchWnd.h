#pragma once

#include "EasyToolBar.h"
#include "SearchEdit.h"
#define WM_EDIT_RETURN_MSG WM_USER+3333
class CIE8SearchWnd : public CControlBar
{
	DECLARE_DYNAMIC(CIE8SearchWnd)
protected:
	CSearchEdit     m_editSearch;
	CFont           m_defaultfont;
	CRect           m_rcInput;
	HICON           m_hicoSearch;
	CEasyToolBar    m_tbSearch;

public:
	CIE8SearchWnd();
	virtual ~CIE8SearchWnd();
	void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
	{

	}

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg LRESULT OnEditReturn(WPARAM wParam,LPARAM lParam);
	CString GetSearchText(void);
};


