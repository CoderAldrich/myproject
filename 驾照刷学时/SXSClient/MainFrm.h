
// MainFrm.h : CMainFrame ��Ľӿ�
//

#pragma once

#include "SXSView.h"

class CMainFrame : public CFrameWnd
{
public: // �������л�����
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)
	CSXSView *m_pView;
	
// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	IDispatch *CreateInstance(LPCTSTR pszUrl);
	HWND GetIEServerWnd();
	VOID SetVirtualMousePos(CPoint ptPos);
// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcDestroy();
};


