
// TestBrowserView.cpp : CTestBrowserView ���ʵ��
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
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CHtmlView::OnFilePrint)
END_MESSAGE_MAP()

// CTestBrowserView ����/����

CTestBrowserView::CTestBrowserView()
{
	// TODO: �ڴ˴���ӹ������

}

CTestBrowserView::~CTestBrowserView()
{
}

BOOL CTestBrowserView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CHtmlView::PreCreateWindow(cs);
}

void CTestBrowserView::OnInitialUpdate()
{
	CHtmlView::OnInitialUpdate();

	Navigate2(_T("http://www.qq.com/"),NULL,NULL);
}


// CTestBrowserView ��ӡ



// CTestBrowserView ���

#ifdef _DEBUG
void CTestBrowserView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CTestBrowserView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}

CTestBrowserDoc* CTestBrowserView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTestBrowserDoc)));
	return (CTestBrowserDoc*)m_pDocument;
}
#endif //_DEBUG


// CTestBrowserView ��Ϣ�������
