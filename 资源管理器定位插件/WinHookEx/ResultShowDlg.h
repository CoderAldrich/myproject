#pragma once
#include "resource.h"


// CResultShowDlg 对话框

class CResultShowDlg : public CDialog
{
	DECLARE_DYNAMIC(CResultShowDlg)

public:
	CResultShowDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CResultShowDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
