#pragma once



class CSimpleFrameWnd : public CWnd
{
	DECLARE_DYNCREATE(CSimpleFrameWnd)

public:
	CSimpleFrameWnd();

	virtual BOOL Create(LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName,
		DWORD dwStyle = WS_VISIBLE|WS_CHILD,
		const RECT& rect = CRect(0,0,800,600),
		CWnd* pParentWnd = NULL,        // != NULL for popups
		DWORD dwExStyle = 0);
public:
	virtual ~CSimpleFrameWnd();
protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


