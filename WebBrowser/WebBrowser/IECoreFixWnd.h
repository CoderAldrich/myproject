#pragma once


// CIECoreFixWnd

class CIECoreFixWnd : public CWnd
{
	DECLARE_DYNAMIC(CIECoreFixWnd)
protected:
	HWND m_hWndZoomNotify;
	UINT m_nZoomMsg;
public:
	CIECoreFixWnd();
	virtual ~CIECoreFixWnd();
public:
	VOID SetZoomNotifyWindow(HWND hWnd,UINT nMsg);
protected:
    afx_msg LRESULT OnGetObject( WPARAM wParam,LPARAM lParam );
    afx_msg LRESULT OnHtmlGetObject( WPARAM wParam,LPARAM lParam );
	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


