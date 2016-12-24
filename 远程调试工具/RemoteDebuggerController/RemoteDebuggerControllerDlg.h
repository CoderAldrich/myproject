
// RemoteDebuggerControllerDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

// CRemoteDebuggerControllerDlg 对话框
class CRemoteDebuggerControllerDlg : public CDialog
{
// 构造
public:
	CRemoteDebuggerControllerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_REMOTEDEBUGGERCONTROLLER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
public:

	HANDLE m_hClient;
// 实现
protected:
	HICON m_hIcon;
	HANDLE m_hCurClient;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	LRESULT OnHandleRecvMsg(WPARAM wParam,LPARAM lParam);
	CString m_strCmdLine;
	afx_msg void OnBnClickedOk();
	CEdit m_wndMsgShow;
	CListBox m_wndOnLineClient;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	VOID RefushOnClient(void);
	afx_msg void OnLbnSelchangeList1();
	CEdit m_wndCurClient;
	afx_msg void OnBnClickedButton1();
};
