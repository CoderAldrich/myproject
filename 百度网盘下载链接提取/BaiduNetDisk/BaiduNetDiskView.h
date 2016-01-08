#pragma once

#ifdef _WIN32_WCE
#error "Windows CE 不支持 CAutoBrowserView。"
#endif 

#include "AutoBrowserView.h"





class CBaiduNetDiskView : public CAutoBrowserView
{
	DECLARE_DYNCREATE(CBaiduNetDiskView)

public:

protected:

	int   m_nMoveCount;
	BOOL  m_bClickSuccess;
	int   m_nClickTryTimes;


	CString m_strMatrixUrl;

	int m_nBeforeClickTime;
	int m_nAdStayTime;
	BOOL m_bAdClick;
	
public:
	CBaiduNetDiskView();           // 动态创建所使用的受保护的构造函数
	virtual ~CBaiduNetDiskView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
public:

	BOOL StartWork();
protected:	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	VOID CalcYXJClickPoint(CPoint &ptClick);
	VOID CalcXLTClickPoint(CPoint &ptClick);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void NewWindow3( IDispatch **ppDisp,VARIANT_BOOL *Cancel,DWORD dwFlags,BSTR bstrUrlContext,BSTR bstrUrl);
	void OnMainDocumentComplete(LPDISPATCH pDisp, VARIANT* URL);

	HRESULT OnDownloadFile( BSTR bstrFileUrl );
};


