#pragma once


// CAddMusicDlg �Ի���

class CAddMusicDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddMusicDlg)
protected:
	CString m_strFileName;
public:
	CAddMusicDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAddMusicDlg();

// �Ի�������
	enum { IDD = IDD_ADD_MUSIC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	VOID InitDisplay(LPCWSTR pszFileName);
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

	CString GetMusicDescription();

	CString m_strMusicDescription;
};
