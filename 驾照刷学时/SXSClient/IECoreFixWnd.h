#pragma once


// CIECoreFixWnd

class CIECoreFixWnd : public CWnd
{
	DECLARE_DYNAMIC(CIECoreFixWnd)

public:
	CIECoreFixWnd();
	virtual ~CIECoreFixWnd();

protected:
    afx_msg LRESULT OnGetObject( WPARAM wParam,LPARAM lParam );
    afx_msg LRESULT OnHtmlGetObject( WPARAM wParam,LPARAM lParam );
	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


