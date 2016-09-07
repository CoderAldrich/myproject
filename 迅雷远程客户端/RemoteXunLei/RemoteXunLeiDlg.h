
// RemoteXunLeiDlg.h : 头文件
//

#pragma once

#include "XunLeiClient.h"
// CRemoteXunLeiDlg 对话框
class CRemoteXunLeiDlg : public CDialog
{
// 构造
public:
	CRemoteXunLeiDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_REMOTEXUNLEI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:

	CXunLeiDownloader *m_pDownloaders[10];

	HICON m_hIcon;
	CStringA m_strPid;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	CString m_strMsgOut;
};
