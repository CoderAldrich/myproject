#pragma once


//#include "ACEditSubclassWnd.h"
class CIEAddrEdit : public CEdit
{
	DECLARE_DYNAMIC(CIEAddrEdit)

public:
	CIEAddrEdit();
	virtual ~CIEAddrEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
	LRESULT OnItemActive(WPARAM wParam,LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CString FormatUrl(CString strOrgUrl);
	BOOL SubclassWindow(HWND hWnd);
	afx_msg void OnEnKillfocus();
	afx_msg void OnEnSetfocus();
	afx_msg void OnEnUpdate();
};


