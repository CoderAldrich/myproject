
// DllInjecterDlg.h : ͷ�ļ�
//

#pragma once


// CDllInjecterDlg �Ի���
class CDllInjecterDlg : public CDialog
{
// ����
public:
	CDllInjecterDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_DLLINJECTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
