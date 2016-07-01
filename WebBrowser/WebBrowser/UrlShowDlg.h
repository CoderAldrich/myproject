#pragma once

#include "resource.h"
// CUrlShowDlg 对话框

class CUrlShowDlg : public CDialog
{
	DECLARE_DYNAMIC(CUrlShowDlg)

public:
	CUrlShowDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUrlShowDlg();

// 对话框数据
	enum { IDD = IDD_DLG_URL_SHOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strUrl;
};
