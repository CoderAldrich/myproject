#pragma once


// CIE8ReBar

class CIE8ReBar : public CReBar
{
	DECLARE_DYNAMIC(CIE8ReBar)

public:
	CIE8ReBar();
	virtual ~CIE8ReBar();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


