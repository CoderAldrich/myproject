
// CNZZExportDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CCNZZExportDlg 对话框
class CCNZZExportDlg : public CDialog
{
// 构造
public:
	CCNZZExportDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CNZZEXPORT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	LRESULT OnShowText( WPARAM wParam,LPARAM lParam );

	CString m_strCNZZID;
	CString m_strCNZZPassWord;
	CString m_strExportDayTime;
	afx_msg void OnBnClickedOk();
	CEdit m_editMsgShow;
	CString m_strExportSavePath;
};
