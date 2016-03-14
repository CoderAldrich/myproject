#pragma once


// CAddMusicDlg 对话框

class CAddMusicDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddMusicDlg)
protected:
	CString m_strFileName;
public:
	CAddMusicDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAddMusicDlg();

// 对话框数据
	enum { IDD = IDD_ADD_MUSIC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	VOID InitDisplay(LPCWSTR pszFileName);
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

	CString GetMusicDescription();

	CString m_strMusicDescription;
};
