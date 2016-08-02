// MyEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "JsInjector.h"
#include "MyEdit.h"


// CMyEdit

IMPLEMENT_DYNAMIC(CMyEdit, CEdit)

CMyEdit::CMyEdit()
{

}

CMyEdit::~CMyEdit()
{
}


BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
END_MESSAGE_MAP()



// CMyEdit 消息处理程序



BOOL CMyEdit::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		BOOL b = GetKeyState(VK_CONTROL) & 0x80;
		if(b && (pMsg->wParam=='a'||pMsg->wParam=='A'))
		{
			SetSel(0,-1);
			return TRUE;
		}
	}
	return CEdit::PreTranslateMessage(pMsg);
}
