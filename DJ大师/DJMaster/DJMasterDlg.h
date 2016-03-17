
// DJMasterDlg.h : ͷ�ļ�
//

#pragma once

#include "MusicDisplayWnd.h"
#include "afxcmn.h"

#include "MCI.h"
#include "VolumeCtrl.h"

typedef struct tagMUSIC_NOTE
{
	CString strFilePath;
}MUSIC_NOTE,*PMUSIC_NOTE;
// CDJMasterDlg �Ի���
class CDJMasterDlg : public CDialog
{
// ����
public:
	CDJMasterDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_DJMASTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	BOOL             m_bEnableEdit;
	HICON            m_hIcon;
	CMusicDisplayWnd m_wndMusicDisplay;
	CVolumeCtrl      m_wndVolumeCtrl;
	CMCI             m_Mci;

	// ���ɵ���Ϣӳ�亯��
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
	LRESULT OnMusicChange(WPARAM wParam,LPARAM lParam);
	LRESULT OnVolumeChange(WPARAM wParam,LPARAM lParam);
	void RelayoutChild(int cx, int cy);
	BOOL AddMusicFile(LPCWSTR pszFilePath);
};
