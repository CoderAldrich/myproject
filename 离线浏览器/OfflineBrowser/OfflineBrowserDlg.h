
// OfflineBrowserDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"


// COfflineBrowserDlg �Ի���
class COfflineBrowserDlg : public CDialog
{
// ����
public:
	COfflineBrowserDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_OFFLINEBROWSER_DIALOG };

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
	CTreeCtrl m_trWebTree;
	afx_msg void OnBnClickedOk();
};
