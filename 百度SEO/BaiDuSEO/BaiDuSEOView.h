#pragma once
#include "IECoreView.h"
#ifdef _WIN32_WCE
#error "Windows CE ��֧�� CHtmlView��"
#endif 

// CBaiDuSEOView Html ��ͼ

class CBaiDuSEOView : public CIECoreView
{
	DECLARE_DYNCREATE(CBaiDuSEOView)

public:
	CBaiDuSEOView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CBaiDuSEOView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	BOOL WaitDocumentComplete(DWORD dwTimeOut);
	virtual void NewWindow3( IDispatch **ppDisp,VARIANT_BOOL *Cancel,DWORD dwFlags,BSTR bstrUrlContext,BSTR bstrUrl);
};


