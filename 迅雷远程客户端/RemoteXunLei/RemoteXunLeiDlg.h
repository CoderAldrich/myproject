
// RemoteXunLeiDlg.h : ͷ�ļ�
//

#pragma once

#include "XunLeiClient.h"
// CRemoteXunLeiDlg �Ի���
class CRemoteXunLeiDlg : public CDialog
{
// ����
public:
	CRemoteXunLeiDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_REMOTEXUNLEI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:

	CXunLeiDownloader *m_pDownloaders[10];

	HICON m_hIcon;
	CStringA m_strPid;
	// ���ɵ���Ϣӳ�亯��
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
