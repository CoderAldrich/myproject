// ResultShowDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WinHookEx.h"
#include "ResultShowDlg.h"


// CResultShowDlg 对话框

IMPLEMENT_DYNAMIC(CResultShowDlg, CDialog)

CResultShowDlg::CResultShowDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CResultShowDlg::IDD, pParent)
{

}

CResultShowDlg::~CResultShowDlg()
{
}

void CResultShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CResultShowDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CResultShowDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CResultShowDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CResultShowDlg 消息处理程序

void CResultShowDlg::OnBnClickedOk()
{
}

void CResultShowDlg::OnBnClickedCancel()
{
}
