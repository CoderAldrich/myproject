
// TestBrowserDoc.h : CTestBrowserDoc ��Ľӿ�
//


#pragma once


class CTestBrowserDoc : public CDocument
{
protected: // �������л�����
	CTestBrowserDoc();
	DECLARE_DYNCREATE(CTestBrowserDoc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CTestBrowserDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};


