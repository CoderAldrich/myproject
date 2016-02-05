#pragma once

class CSurfaceWnd:public CWnd
{
private:
	BOOL  m_bTracking;
	bool m_bMouseDown;
	HWND m_hWndNotify;
	UINT m_nMsgNotify;
public:
	CSurfaceWnd()
		: m_bMouseDown(false)
	{
		m_bTracking = FALSE;
		m_hWndNotify  =NULL;
		m_nMsgNotify =NULL;
	}
	virtual ~CSurfaceWnd()
	{

	}

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	void RegisterStatusChangeNotify(HWND hWnd,UINT nMsg);

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};