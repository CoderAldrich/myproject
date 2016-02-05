#pragma once
#include "SurfaceWnd.h"
#include "EasyToolBar.h"
class CIE8NaviBar : public CEasyToolBar
{
	DECLARE_DYNAMIC(CIE8NaviBar)

public:
	CSurfaceWnd m_wndDropDown;
	CIE8NaviBar();
	virtual ~CIE8NaviBar();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTbnDropDown(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTbnHotItemChange(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
};
