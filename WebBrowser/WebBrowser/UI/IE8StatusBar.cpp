// IE8StatusBar.cpp : 实现文件
//

#include "stdafx.h"
#include "UIPublic.h"
#include "IE8StatusBar.h"
#include "BrowserPublic.h"

// CIE8StatusBar

IMPLEMENT_DYNAMIC(CIE8StatusBar, CStatusBar)

CIE8StatusBar::CIE8StatusBar()
{

}

CIE8StatusBar::~CIE8StatusBar()
{
}


BEGIN_MESSAGE_MAP(CIE8StatusBar, CStatusBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CIE8StatusBar 消息处理程序

int CIE8StatusBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatusBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_tbZoom.CreateEx(this,TBSTYLE_FLAT|TBSTYLE_LIST|TBSTYLE_TRANSPARENT|TBSTYLE_AUTOSIZE);
	m_tbZoom.SetButtonBorder(true);

	m_tbZoom.SetImageList(IDB_IE8_CMD,16,16);

	m_tbZoom.GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);

	m_tbZoom.GetToolBarCtrl().SetBitmapSize(CSize(12,12));

	m_tbZoom.AddButton(0,ID_NOTHING_ENABLE,TBBS_BUTTON|TBBS_AUTOSIZE|TBBS_DROPDOWN,TBSTATE_ENABLED,5);
	
	if(GetIsWindowBaseTheme() == false)
	{
		m_tbZoom.SetButtonPad(0,0);
		m_tbZoom.SetButtonSpace(0);
	}

	m_tbZoom.SetButtonText(0,TEXT("100%"));

	return 0;
}

void CIE8StatusBar::DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
{
}
void CIE8StatusBar::OnSize(UINT nType, int cx, int cy)
{
	CStatusBar::OnSize(nType, cx, cy);

	CRect rcItem;
	GetItemRect(8,&rcItem);
	rcItem.top = -1;

	CRect rcTbItem;
	m_tbZoom.GetToolBarCtrl().GetItemRect(0,&rcTbItem);
	rcItem.top = rcTbItem.top;
	rcItem.bottom = rcTbItem.bottom;

	m_tbZoom.MoveWindow(rcItem);
}

BOOL CIE8StatusBar::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style|=WS_CLIPCHILDREN;
	return CStatusBar::PreCreateWindow(cs);
}
