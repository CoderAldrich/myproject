#pragma once

#ifdef _WIN32_WCE
#error "Windows CE 不支持 CAutoBrowserView。"
#endif 

#include "AutoBrowserView.h"

class CPhoneView : public CAutoBrowserView
{
	DECLARE_DYNCREATE(CPhoneView)
private:
	static BOOL bHook;
	static BOOL bInternalHook;
public:
	static CString m_strUserAgent;
	static CString m_strPlatform;
public:
	CPhoneView();           // 动态创建所使用的受保护的构造函数
	virtual ~CPhoneView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
public:
	VOID SetUserAgentString(LPCWSTR pszUserAgent);
	VOID SetPlatformString(LPCWSTR pszPlatform);
protected:	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual void OnInitialUpdate();
	virtual void OnMainDocumentComplete(LPDISPATCH pDisp, VARIANT* URL);
};


