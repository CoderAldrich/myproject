#pragma once

#include "IECoreView.h"

#ifdef _WIN32_WCE
#error "Windows CE 不支持 CIECoreView。"
#endif 

// CSXSView Html 视图

class CSXSView : public CIECoreView
{
	DECLARE_DYNCREATE(CSXSView)

public:
	CSXSView();           // 动态创建所使用的受保护的构造函数
	virtual ~CSXSView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	void DocumentComplete(LPDISPATCH pDisp, VARIANT* URL);

	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


