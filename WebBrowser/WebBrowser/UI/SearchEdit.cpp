// SearchEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "SearchEdit.h"


// CSearchEdit

IMPLEMENT_DYNAMIC(CSearchEdit, CEdit)

CSearchEdit::CSearchEdit()
{
	m_msgReturnToParent = 0;
}

CSearchEdit::~CSearchEdit()
{
}


BEGIN_MESSAGE_MAP(CSearchEdit, CEdit)
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()



// CSearchEdit 消息处理程序

void CSearchEdit::SetNotifyMessageOnReturn(UINT nMsg)
{
	m_msgReturnToParent = nMsg;
}
void  CSearchEdit::OnSetFocus(CWnd* pOldWnd)
{
	CEdit::OnSetFocus(pOldWnd);

	
}
BOOL CSearchEdit::PreTranslateMessage(MSG* pMsg)
{
	if (m_msgReturnToParent && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
	{
		GetParent()->PostMessage(m_msgReturnToParent,0,0);
		return true;
	}

	if (pMsg->message == WM_LBUTTONDOWN && GetFocus() != this )
	{
		SetFocus();
		SetSel(0,-1);
		return true;
	}

	return CEdit::PreTranslateMessage(pMsg);
}
