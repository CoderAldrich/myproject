
// MainFrm.h : CMainFrame ��Ľӿ�
//

#pragma once

#include "IECoreView.h"

class CMainFrame : public CFrameWnd
{
public: // �������л�����
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)
	CIECoreView *m_pView;

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	IDispatch *CreateInstance();

public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcDestroy();
	afx_msg void OnClose();
};


