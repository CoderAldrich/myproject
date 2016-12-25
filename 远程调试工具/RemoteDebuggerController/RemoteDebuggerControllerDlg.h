
// RemoteDebuggerControllerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// CRemoteDebuggerControllerDlg �Ի���
class CRemoteDebuggerControllerDlg : public CDialog
{
// ����
public:
	CRemoteDebuggerControllerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_REMOTEDEBUGGERCONTROLLER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
public:

	HANDLE m_hClient;
// ʵ��
protected:
	HICON m_hIcon;
	HANDLE m_hCurClient;
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	LRESULT OnHandleRecvMsg(WPARAM wParam,LPARAM lParam);
	CString m_strCmdLine;
	afx_msg void OnBnClickedOk();
	CEdit m_wndMsgShow;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	VOID RefushOnClient(void);
	CEdit m_wndCurClient;
	afx_msg void OnBnClickedButton1();
	CListCtrl m_wndOnlineClient;
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
};
