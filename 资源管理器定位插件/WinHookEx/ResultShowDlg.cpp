// ResultShowDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WinHookEx.h"
#include "ResultShowDlg.h"


// CResultShowDlg �Ի���

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


// CResultShowDlg ��Ϣ�������

void CResultShowDlg::OnBnClickedOk()
{
}

void CResultShowDlg::OnBnClickedCancel()
{
}
