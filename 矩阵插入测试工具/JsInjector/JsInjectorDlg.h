
// JsInjectorDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CJsInjectorDlg �Ի���
class CJsInjectorDlg : public CDialog
{
// ����
public:
	CJsInjectorDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_JSINJECTOR_DIALOG };

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
	LRESULT OnDebugMsg(WPARAM ,LPARAM);
	afx_msg void OnBnClickedOk();
	CString m_strJsData;
	afx_msg void OnEnChangeEdit1();
	CListBox m_wndListBox;
	afx_msg void OnBnClickedButton1();
};
