#pragma once
#include "IECoreView.h"
#ifdef _WIN32_WCE
#error "Windows CE 不支持 CHtmlView。"
#endif 

// CBaiDuSEOView Html 视图

class CBaiDuSEOView : public CIECoreView
{
	DECLARE_DYNCREATE(CBaiDuSEOView)

public:
	CBaiDuSEOView();           // 动态创建所使用的受保护的构造函数
	virtual ~CBaiDuSEOView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	BOOL WaitDocumentComplete(DWORD dwTimeOut);
	virtual void NewWindow3( IDispatch **ppDisp,VARIANT_BOOL *Cancel,DWORD dwFlags,BSTR bstrUrlContext,BSTR bstrUrl);
};


