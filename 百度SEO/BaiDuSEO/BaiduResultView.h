#pragma once

#ifdef _WIN32_WCE
#error "Windows CE 不支持 CAutoBrowserView。"
#endif 

#include "AutoBrowserView.h"

class CBaiduResultView : public CAutoBrowserView
{
	DECLARE_DYNCREATE(CBaiduResultView)

public:
	CBaiduResultView();           // 动态创建所使用的受保护的构造函数
	virtual ~CBaiduResultView();

	BOOL m_bClickElem;
	UINT m_nClickElemCount;
	UINT m_nMaxClickElemCount;


public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
public:

	VOID SetWorkParam(UINT nMaxClickElemCount);
	virtual BOOL StartWork();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void OnNewWindow3( IDispatch **ppDisp,VARIANT_BOOL *Cancel,DWORD dwFlags,BSTR bstrUrlContext,BSTR bstrUrl);
};


