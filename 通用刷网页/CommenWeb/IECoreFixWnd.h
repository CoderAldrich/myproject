#pragma once


// CIECoreFixWnd

class CIECoreFixWnd : public CWnd
{
	DECLARE_DYNAMIC(CIECoreFixWnd)

public:
	CIECoreFixWnd();
	virtual ~CIECoreFixWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


