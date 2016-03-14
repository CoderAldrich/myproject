// AddMusicDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DJMaster.h"
#include "AddMusicDlg.h"


// CAddMusicDlg 对话框

IMPLEMENT_DYNAMIC(CAddMusicDlg, CDialog)

CAddMusicDlg::CAddMusicDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddMusicDlg::IDD, pParent)
	, m_strMusicDescription(_T(""))
{

}

CAddMusicDlg::~CAddMusicDlg()
{
}

void CAddMusicDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strMusicDescription);
}


BEGIN_MESSAGE_MAP(CAddMusicDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAddMusicDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CAddMusicDlg 消息处理程序

VOID CAddMusicDlg::InitDisplay(LPCWSTR pszFileName)
{
	m_strFileName = pszFileName;
	return VOID();
}


CString CAddMusicDlg::GetMusicDescription()
{
	return m_strMusicDescription;
}

void CAddMusicDlg::OnBnClickedOk()
{
	UpdateData();
	OnOK();
}

BOOL CAddMusicDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_STATIC_FILENAME)->SetWindowText(m_strFileName);

	CString strDefFileDesc;
	strDefFileDesc = m_strFileName.Right(m_strFileName.GetLength() - m_strFileName.ReverseFind(L'\\')-1);

	m_strMusicDescription = strDefFileDesc;
	UpdateData(FALSE);

	return TRUE;
}
