
// SuperYueCheDlg.h : 头文件
//

#pragma once

#include "ServerClient.h"
#include "afxwin.h"

// CSuperYueCheDlg 对话框
class CSuperYueCheDlg : public CDialog
{
// 构造
public:
	CSuperYueCheDlg(CWnd* pParent = NULL);	// 标准构造函数
	virtual ~CSuperYueCheDlg();
// 对话框数据
	enum { IDD = IDD_SUPERYUECHE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

 	CString m_strID;
 	CString m_strPassWord;
 	CDateTimeCtrl m_wndDataTime; 


// 实现
protected:
	HICON m_hIcon;
	CServerClient Client;

	CRITICAL_SECTION m_cs;
	HANDLE m_hYueCheThread;

	VOID LoadConfig();
	VOID SaveConfig();
	

	VOID GetWorkTime(LPSYSTEMTIME pTime);
	VOID GetSelTime(BOOL * pTime);

	void OnWeekChange(void);
	void OnDtnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult);
	
	static DWORD WINAPI YueCheThread(PVOID pParam);

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	CEdit m_wndMsgEdit;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	void AddMessageShow(CString strMessage);
	LRESULT OnYueCheThreadExit(WPARAM wParam,LPARAM lParam);
};
