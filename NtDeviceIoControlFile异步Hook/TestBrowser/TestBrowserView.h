
// TestBrowserView.h : CTestBrowserView 类的接口
//


#pragma once


class CTestBrowserView : public CHtmlView
{
protected: // 仅从序列化创建
	CTestBrowserView();
	DECLARE_DYNCREATE(CTestBrowserView)

// 属性
public:
	CTestBrowserDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CTestBrowserView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // TestBrowserView.cpp 中的调试版本
inline CTestBrowserDoc* CTestBrowserView::GetDocument() const
   { return reinterpret_cast<CTestBrowserDoc*>(m_pDocument); }
#endif

