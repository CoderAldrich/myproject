
// CDNTesterDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CCDNTesterDlg �Ի���
class CCDNTesterDlg : public CDialog
{
// ����
public:
	CCDNTesterDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CDNTESTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_strTestUrl;
	CEdit m_wndReqAppendHeaders;
	CEdit m_wndTestIps;
};
