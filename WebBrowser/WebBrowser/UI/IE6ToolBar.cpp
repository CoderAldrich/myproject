
#include "stdafx.h"
#include "IE6ToolBar.h"
CIE6ToolBar::CIE6ToolBar()
{

};
CIE6ToolBar::~CIE6ToolBar()
{

}
BEGIN_MESSAGE_MAP(CIE6ToolBar, CEasyToolBar)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int CIE6ToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEasyToolBar::OnCreate(lpCreateStruct) == -1)
	
		return -1;


	GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);

	UINT nBitmapIDs[]={0,1,2,3,4,I_IMAGENONE,5,6,12,I_IMAGENONE,13,7,9};

	UINT nIDs[13]=
	{
		ID_GO_BACK,
		ID_GO_FORWARD,
		ID_STOP_LOAD,
		ID_REFREASH,
		ID_GO_HOME,
		ID_NOTHING_ENABLE,
		ID_NOTHING_ENABLE,
		ID_NOTHING_ENABLE,
		ID_NOTHING_ENABLE,
		ID_NOTHING_ENABLE,
		ID_NOTHING_ENABLE,
		ID_NOTHING_ENABLE,
		ID_NOTHING_ENABLE
	};

	for (int i=0;i<13;i++)
	{
		if(i == 5 || i == 9)
		{
			AddButton(i,nIDs[i],TBBS_SEPARATOR,TBSTATE_ENABLED,nBitmapIDs[i]);
		}
		else
		{
			AddButton(i,nIDs[i],TBBS_BUTTON|TBBS_AUTOSIZE,TBSTATE_ENABLED,nBitmapIDs[i]);
		}
	
		//AddButton(i,ID_FILE_NEW,TBBS_BUTTON/*|TBBS_AUTOSIZE*/,TBSTATE_ENABLED,nBitmapIDs[i]);
	}

	//return 0;

	SetButtonStyle(0,GetButtonStyle(0)|TBBS_DROPDOWN|TBBS_AUTOSIZE);
	SetButtonStyle(1,GetButtonStyle(1)|TBBS_DROPDOWN|TBBS_AUTOSIZE);
	SetButtonStyle(6,GetButtonStyle(6)|TBBS_AUTOSIZE);
	SetButtonStyle(7,GetButtonStyle(7)|TBBS_AUTOSIZE);

	//SetButtonStyle(5,GetButtonStyle(5)|TBBS_SEPARATOR&~TBBS_BUTTON&~TBBS_AUTOSIZE);
	//SetButtonStyle(9,GetButtonStyle(9)|TBBS_SEPARATOR&~TBBS_BUTTON&~TBBS_AUTOSIZE);
	SetButtonStyle(10,GetButtonStyle(10)|BTNS_WHOLEDROPDOWN/*|TBBS_AUTOSIZE*/);


	SetButtonText(0,TEXT("ºóÍË"));
	SetButtonText(6,TEXT("ËÑË÷"));
	SetButtonText(7,TEXT("ÊÕ²Ø¼Ð"));

	SetImageList(IDB_IE6_TOOLBAR,24,24);
	SetButtonBorder(TRUE);

	SetSizes(CSize(32,38),CSize(24,24));

	return 0;
}
