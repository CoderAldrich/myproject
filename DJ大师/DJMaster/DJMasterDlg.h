
// DJMasterDlg.h : 头文件
//

#pragma once

#include "MusicDisplayWnd.h"
#include "afxcmn.h"

#include "MCI.h"
#include "VolumeCtrl.h"

// CDJMasterDlg 对话框
class CDJMasterDlg : public CDialog
{
// 构造
public:
	CDJMasterDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DJMASTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	BOOL             m_bEnableEdit;
	HICON            m_hIcon;
	CMusicDisplayWnd m_wndMusicDisplay;
	CVolumeCtrl      m_wndVolumeCtrl;
	CMCI             m_Mci;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	void RelayoutChild(int cx, int cy);
	BOOL AddMusicFile(LPCWSTR pszFilePath);
};
