#pragma once

#include "resource.h"
// CUrlShowDlg �Ի���

class CUrlShowDlg : public CDialog
{
	DECLARE_DYNAMIC(CUrlShowDlg)

public:
	CUrlShowDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CUrlShowDlg();

// �Ի�������
	enum { IDD = IDD_DLG_URL_SHOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_strUrl;
};
