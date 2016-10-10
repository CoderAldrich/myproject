
// TestBrowserView.cpp : CTestBrowserView 类的实现
//

#include "stdafx.h"
#include "TestBrowser.h"

#include "TestBrowserDoc.h"
#include "TestBrowserView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestBrowserView

IMPLEMENT_DYNCREATE(CTestBrowserView, CHtmlView)

BEGIN_MESSAGE_MAP(CTestBrowserView, CHtmlView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CHtmlView::OnFilePrint)
END_MESSAGE_MAP()

// CTestBrowserView 构造/析构

CTestBrowserView::CTestBrowserView()
{
	// TODO: 在此处添加构造代码

}

CTestBrowserView::~CTestBrowserView()
{
}

BOOL CTestBrowserView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CHtmlView::PreCreateWindow(cs);
}

void CTestBrowserView::OnInitialUpdate()
{
	CHtmlView::OnInitialUpdate();

	Navigate2(_T("http://www.qq.com/"),NULL,NULL);
}


// CTestBrowserView 打印



// CTestBrowserView 诊断

#ifdef _DEBUG
void CTestBrowserView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CTestBrowserView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}

CTestBrowserDoc* CTestBrowserView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTestBrowserDoc)));
	return (CTestBrowserDoc*)m_pDocument;
}
#endif //_DEBUG


// CTestBrowserView 消息处理程序
