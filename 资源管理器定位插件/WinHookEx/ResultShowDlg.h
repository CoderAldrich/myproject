#pragma once
#include "resource.h"


// CResultShowDlg �Ի���

class CResultShowDlg : public CDialog
{
	DECLARE_DYNAMIC(CResultShowDlg)

public:
	CResultShowDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CResultShowDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
