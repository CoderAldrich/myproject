
// MainFrm.h : CMainFrame 类的接口
//

#pragma once

#include "SXSView.h"

class CMainFrame : public CFrameWnd
{
public: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)
	CSXSView *m_pView;
	
// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	IDispatch *CreateInstance(LPCTSTR pszUrl);
	HWND GetIEServerWnd();
	VOID SetVirtualMousePos(CPoint ptPos);
// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcDestroy();
};


