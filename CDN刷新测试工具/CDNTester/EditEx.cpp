// EditEx.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CDNTester.h"
#include "EditEx.h"


// CEditEx

IMPLEMENT_DYNAMIC(CEditEx, CEdit)

CEditEx::CEditEx()
{

}

CEditEx::~CEditEx()
{
}


BEGIN_MESSAGE_MAP(CEditEx, CEdit)
END_MESSAGE_MAP()



// CEditEx ��Ϣ�������



BOOL CEditEx::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if ( GetAsyncKeyState(VK_CONTROL) && (pMsg->wParam == 'A' || pMsg->wParam == 'a') )
		{
			this->SetSel(0,this->GetWindowTextLength(),TRUE);
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}
