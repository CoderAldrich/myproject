
// SuperYueCheDlg.h : ͷ�ļ�
//

#pragma once

#include "ServerClient.h"
#include "afxwin.h"

// CSuperYueCheDlg �Ի���
class CSuperYueCheDlg : public CDialog
{
// ����
public:
	CSuperYueCheDlg(CWnd* pParent = NULL);	// ��׼���캯��
	virtual ~CSuperYueCheDlg();
// �Ի�������
	enum { IDD = IDD_SUPERYUECHE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

 	CString m_strID;
 	CString m_strPassWord;
 	CDateTimeCtrl m_wndDataTime; 


// ʵ��
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

	// ���ɵ���Ϣӳ�亯��
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
