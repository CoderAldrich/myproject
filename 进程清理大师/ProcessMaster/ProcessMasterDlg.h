
// ProcessMasterDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CProcessMasterDlg �Ի���
class CProcessMasterDlg : public CDialog
{
// ����
public:
	CProcessMasterDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_PROCESSMASTER_DIALOG };

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
	CListBox m_wndItemList;
	afx_msg void OnBnClickedButton1();
	CString m_strProcessName;
	CString m_strDependProcessName;
	afx_msg void OnBnClickedOk();
};
