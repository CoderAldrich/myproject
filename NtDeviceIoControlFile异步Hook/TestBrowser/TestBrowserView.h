
// TestBrowserView.h : CTestBrowserView ��Ľӿ�
//


#pragma once


class CTestBrowserView : public CHtmlView
{
protected: // �������л�����
	CTestBrowserView();
	DECLARE_DYNCREATE(CTestBrowserView)

// ����
public:
	CTestBrowserDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // ������һ�ε���

// ʵ��
public:
	virtual ~CTestBrowserView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // TestBrowserView.cpp �еĵ��԰汾
inline CTestBrowserDoc* CTestBrowserView::GetDocument() const
   { return reinterpret_cast<CTestBrowserDoc*>(m_pDocument); }
#endif

