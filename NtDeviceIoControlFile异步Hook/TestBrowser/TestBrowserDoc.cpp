
// TestBrowserDoc.cpp : CTestBrowserDoc ���ʵ��
//

#include "stdafx.h"
#include "TestBrowser.h"

#include "TestBrowserDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestBrowserDoc

IMPLEMENT_DYNCREATE(CTestBrowserDoc, CDocument)

BEGIN_MESSAGE_MAP(CTestBrowserDoc, CDocument)
END_MESSAGE_MAP()


// CTestBrowserDoc ����/����

CTestBrowserDoc::CTestBrowserDoc()
{
	// TODO: �ڴ����һ���Թ������

}

CTestBrowserDoc::~CTestBrowserDoc()
{
}

BOOL CTestBrowserDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CTestBrowserDoc ���л�

void CTestBrowserDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}


// CTestBrowserDoc ���

#ifdef _DEBUG
void CTestBrowserDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTestBrowserDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CTestBrowserDoc ����
