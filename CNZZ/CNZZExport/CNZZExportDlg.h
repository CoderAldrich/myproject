
// CNZZExportDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CCNZZExportDlg �Ի���
class CCNZZExportDlg : public CDialog
{
// ����
public:
	CCNZZExportDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CNZZEXPORT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
