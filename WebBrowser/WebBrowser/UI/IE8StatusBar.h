#pragma once


// CIE8StatusBar
#include "EasyToolBar.h"
class CIE8StatusBar : public CStatusBar
{
	DECLARE_DYNAMIC(CIE8StatusBar)
private:
	CEasyToolBar  m_tbZoom;
public:
	CIE8StatusBar();
	virtual ~CIE8StatusBar();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);

	//virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	//virtual CSize CalcDynamicLayout(int nLength, DWORD nMode);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};


