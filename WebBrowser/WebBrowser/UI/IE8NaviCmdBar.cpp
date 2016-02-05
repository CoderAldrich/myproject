// IE8NaviCmdBar.cpp : 实现文件
//

#include "stdafx.h"
#include "UIPublic.h"
#include "IE8NaviCmdBar.h"


// CIE8NaviCmdBar

IMPLEMENT_DYNAMIC(CIE8NaviCmdBar, CEasyToolBar)

CIE8NaviCmdBar::CIE8NaviCmdBar()
{

}

CIE8NaviCmdBar::~CIE8NaviCmdBar()
{
}


BEGIN_MESSAGE_MAP(CIE8NaviCmdBar, CEasyToolBar)
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CIE8NaviCmdBar 消息处理程序


int CIE8NaviCmdBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CToolBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	static const UINT nSize = 22;
	SetImageList(IDB_NAVICMD_NORMAL,nSize,nSize);
	SetHotImageList( IDB_NAVICMD_HOVER ,nSize,nSize);
	SetPressImageList( IDB_NAVICMD_PRESS ,nSize,nSize);

	UINT nIDs[3]=
	{
		ID_GOTO_URL,
		ID_STOP_LOAD,
		ID_REFREASH
	};

 	for (int i=0;i<3;i++)
 	{
		AddButton(i,nIDs[i],TBBS_BUTTON|TBBS_AUTOSIZE,TBSTATE_ENABLED,i);
 	}

	SetSizes(CSize(27,27),CSize(20,20));

	return 0;
}
