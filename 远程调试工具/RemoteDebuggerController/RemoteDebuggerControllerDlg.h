
// RemoteDebuggerControllerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "TcpSocket.h"

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

	CTcpSocket m_tcpSock;
// ʵ��
protected:
	HICON m_hIcon;

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
	CListBox m_wndOnLineClient;
};
