#pragma once

#include "IECoreView.h"

#ifdef _WIN32_WCE
#error "Windows CE ��֧�� CIECoreView��"
#endif 

// CSXSView Html ��ͼ

class CSXSView : public CIECoreView
{
	DECLARE_DYNCREATE(CSXSView)

public:
	CSXSView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CSXSView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

	void DocumentComplete(LPDISPATCH pDisp, VARIANT* URL);

	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


