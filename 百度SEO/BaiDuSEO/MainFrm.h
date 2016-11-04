
// MainFrm.h : CMainFrame 类的接口
//

#pragma once

#include "IECoreView.h"

class CMainFrame : public CFrameWnd
{
public: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)
	CIECoreView *m_pView;

// 重写
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


