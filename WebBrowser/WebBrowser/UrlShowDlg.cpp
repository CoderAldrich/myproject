// UrlShowDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WebBrowser.h"
#include "UrlShowDlg.h"


// CUrlShowDlg �Ի���

IMPLEMENT_DYNAMIC(CUrlShowDlg, CDialog)

CUrlShowDlg::CUrlShowDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUrlShowDlg::IDD, pParent)
	, m_strUrl(_T(""))
{

}

CUrlShowDlg::~CUrlShowDlg()
{
}

void CUrlShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strUrl);
}


BEGIN_MESSAGE_MAP(CUrlShowDlg, CDialog)
END_MESSAGE_MAP()


// CUrlShowDlg ��Ϣ�������
