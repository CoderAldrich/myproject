
// SXSLauncherDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"


// CSXSLauncherDlg �Ի���
class CSXSLauncherDlg : public CDialog
{
// ����
public:
	CSXSLauncherDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SXSLAUNCHER_DIALOG };

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
	CListCtrl m_wndUserList;
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
};
