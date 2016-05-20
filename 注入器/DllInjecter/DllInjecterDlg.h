
// DllInjecterDlg.h : 头文件
//

#pragma once


// CDllInjecterDlg 对话框
class CDllInjecterDlg : public CDialog
{
// 构造
public:
	CDllInjecterDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DLLINJECTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	UINT m_nProcessID;
	CString m_strProcessName;
	CString m_strDllPath;
	afx_msg void OnBnClickedButton1();
	CString m_strTestLoad;
};
