// IEAddrEdit.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "IEAddrEdit.h"

const UINT m_nAcItemActivateMsg = ::RegisterWindowMessage(_T("AC_ItemActivate"));

#define SHACF_URLHISTORY                0x00000002
typedef HRESULT (WINAPI *SHAUTOCOMPLETEFN) (HWND hTarget, DWORD dwFlags);


// CIEAddrEdit

IMPLEMENT_DYNAMIC(CIEAddrEdit, CEdit)

CIEAddrEdit::CIEAddrEdit()
{

}

CIEAddrEdit::~CIEAddrEdit()
{
}


BEGIN_MESSAGE_MAP(CIEAddrEdit, CEdit)
	ON_REGISTERED_MESSAGE(m_nAcItemActivateMsg,OnItemActive)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, &CIEAddrEdit::OnEnKillfocus)
	ON_CONTROL_REFLECT(EN_SETFOCUS, &CIEAddrEdit::OnEnSetfocus)
	ON_CONTROL_REFLECT(EN_UPDATE, &CIEAddrEdit::OnEnUpdate)
END_MESSAGE_MAP()

#define IsCTRLpressed() ((GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT) * 8 - 1))) != 0) 


LRESULT CIEAddrEdit::OnItemActive(WPARAM wParam,LPARAM lParam)
{
	GetParentFrame()->SendMessage(WM_COMMAND,ID_GOTO_URL,0);
	return 0;
}

BOOL CIEAddrEdit::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		CString strUrl;
		CString strText;
		GetWindowText(strText);

		if(IsCTRLpressed())
		{
			strUrl = FormatUrl(strText);
			SetWindowText(strUrl);
		}

		GetParentFrame()->SendMessage(WM_COMMAND,ID_GOTO_URL,0);
		return true;
	}
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == 'A' || pMsg->wParam == 'a') && IsCTRLpressed())
	{
		this->SetSel(0,-1);
		return true;
	}

	return CEdit::PreTranslateMessage(pMsg);
}
CString CIEAddrEdit::FormatUrl(CString strOrgUrl)
{

	if(strOrgUrl==TEXT("")) 
	{
		return strOrgUrl;
	}

	
	CString strTemplate1;
	CString strTemplate2;
	CString strTemplate3;
	CString strTail;
	strTemplate1=TEXT("http://www.");
	strTemplate2=TEXT("https://www.");
	strTemplate3=TEXT("www.");
	strTail=TEXT(".com");

	strOrgUrl.MakeLower();
	CString strTempHead;
	CString strTempTail;
	strTempTail=strOrgUrl.Right(strTail.GetLength());

	strTempHead=strOrgUrl.Left(strTemplate1.GetLength());

	if(    strTempTail!=TEXT(".com")
		&& strTempTail!=TEXT(".net")
		&& strTempTail!=TEXT(".org")
		&& strTempTail!=TEXT(".edu")
		&& strTempTail!=TEXT(".gov")
		&& strTempTail!=TEXT(".mil")
		&& strTempTail!=TEXT(".int")
		&& strOrgUrl.Right(3)!=TEXT(".cn")
		&& strOrgUrl.Right(7)!=TEXT(".com.cn")
		&& strOrgUrl.Right(5)!=TEXT(".html")
		&& strOrgUrl.Right(4)!=TEXT(".htm")
		&& strOrgUrl.Right(4)!=TEXT(".asp")
		&& strOrgUrl.Right(4)!=TEXT(".php")
		&& strOrgUrl.Right(3)!=TEXT(".js")
		)
	{
		strOrgUrl=strOrgUrl+strTail;//+TEXT("/");
	}
	if(strTempHead!=strTemplate1)
	{
		strTempHead=strOrgUrl.Left(strTemplate2.GetLength());
		if(strTempHead!=strTemplate2)
		{
			strTempHead=strOrgUrl.Left(strTemplate3.GetLength());
			if(strTempHead!=strTemplate3)
			{
				strOrgUrl=strTemplate1+strOrgUrl;
			}
		}
	}

	return strOrgUrl;
}
BOOL CIEAddrEdit::SubclassWindow(HWND hWnd)
{

	//m_wndACEdit.SubclassWindow(hWnd);

	BOOL bRes =  CEdit::SubclassWindow(hWnd);
	HWND hTarget=hWnd;
	DWORD dwFlags=SHACF_URLHISTORY;

	HINSTANCE hSHLWAPIDLL = NULL;
	if (hSHLWAPIDLL == NULL)
	{
		hSHLWAPIDLL= LoadLibrary(TEXT("shlwapi.dll"));
		if (hSHLWAPIDLL== NULL)
		{
			DWORD errorcode=::GetLastError();
			return 0;
		}
	}
	SHAUTOCOMPLETEFN pSHAC = (SHAUTOCOMPLETEFN)GetProcAddress(hSHLWAPIDLL, "SHAutoComplete");
	HRESULT isok;
	if (pSHAC != NULL)
	{
		isok=SUCCEEDED(pSHAC(hTarget,0x2|0x4));
	}
	FreeLibrary(hSHLWAPIDLL); 



	return bRes;
}

void CIEAddrEdit::OnEnKillfocus()
{
	int a=0;
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CIEAddrEdit::OnEnSetfocus()
{
	int a=0;
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CIEAddrEdit::OnEnUpdate()
{
	int a=0;
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CEdit::OnInitDialog()
	// �������Խ� EM_SETEVENTMASK ��Ϣ���͵��ÿؼ���
	// ͬʱ�� ENM_UPDATE ��־�������㵽 lParam �����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}
