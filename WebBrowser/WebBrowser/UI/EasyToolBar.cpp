// NoButtonBorderToolBar.cpp : 实现文件
//

#include "stdafx.h"
#include "UIPublic.h"
#include "EasyToolBar.h"


// CNoButtonBorderToolBar

IMPLEMENT_DYNAMIC(CEasyToolBar, CToolBar)

CEasyToolBar::CEasyToolBar()
{
	m_bHaveBorder = false;
}

CEasyToolBar::~CEasyToolBar()
{
}


BEGIN_MESSAGE_MAP(CEasyToolBar, CToolBar)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CEasyToolBar::OnNMCustomdraw)
END_MESSAGE_MAP()


bool CEasyToolBar::AddButton(UINT nIndex,UINT nCmdID,BYTE ButtonStyle,BYTE ButtnStatus,int iBmpIndex,int iStringIndex)
{
	TBBUTTON tb;
	tb.fsStyle = ButtonStyle;
	tb.fsState = ButtnStatus;
	tb.iString = iStringIndex;

	tb.idCommand = nCmdID;
	tb.iBitmap = iBmpIndex;
	return GetToolBarCtrl().InsertButton(nIndex,&tb) == TRUE;
}

bool  CEasyToolBar::SetImageList(UINT nBmpResourceID,UINT nWidth,UINT nHeight)
{
	CBitmap tmpBmp;
	if(tmpBmp.LoadBitmap(nBmpResourceID))
	{
		m_imglistNormal.DeleteImageList();
		
		BITMAP bmp;
		tmpBmp.GetBitmap(&bmp);
		if(m_imglistNormal.Create(nWidth,nHeight,ILD_IMAGE|ILD_TRANSPARENT,1, (int)(bmp.bmWidth/nWidth)))
		{
			m_imglistNormal.Add(&tmpBmp,RGB(0,0,0));
			CImageList *pPreImgList =  GetToolBarCtrl().SetImageList(&m_imglistNormal);
			if ( pPreImgList != NULL && pPreImgList != &m_imglistNormal)
			{
				pPreImgList->DeleteImageList();
			}
			return true;
		}
	}
	return false;
}
bool  CEasyToolBar::SetHotImageList(UINT nBmpResourceID,UINT nWidth,UINT nHeight)
{
	CBitmap tmpBmp;
	if(tmpBmp.LoadBitmap(nBmpResourceID))
	{
		m_imglistHot.DeleteImageList();

		BITMAP bmp;
		tmpBmp.GetBitmap(&bmp);
		if(m_imglistHot.Create(nWidth,nHeight,ILD_IMAGE|ILD_TRANSPARENT,1, (int)(bmp.bmWidth/nWidth)))
		{
			m_imglistHot.Add(&tmpBmp,RGB(0,0,0));
			CImageList *pPreImgList =  GetToolBarCtrl().SetHotImageList(&m_imglistHot);
			if ( pPreImgList != NULL && pPreImgList != &m_imglistHot)
			{
				pPreImgList->DeleteImageList();
			}
			return true;
		}
	}
	return false;
}
bool  CEasyToolBar::SetPressImageList(UINT nBmpResourceID,UINT nWidth,UINT nHeight)
{
	CBitmap tmpBmp;
	if(tmpBmp.LoadBitmap(nBmpResourceID))
	{
		m_imglistPress.DeleteImageList();

		BITMAP bmp;
		tmpBmp.GetBitmap(&bmp);
		if(m_imglistPress.Create(nWidth,nHeight,ILD_IMAGE|ILD_TRANSPARENT,1, (int)(bmp.bmWidth/nWidth)))
		{
			m_imglistPress.Add(&tmpBmp,RGB(0,0,0));
			CImageList *pPreImgList =  GetToolBarCtrl().SetPressedImageList(0,&m_imglistPress);
			if ( pPreImgList != NULL && pPreImgList != &m_imglistPress)
			{
				pPreImgList->DeleteImageList();
			}
			return true;
		}
	}
	return false;
}

bool CEasyToolBar::SetDisableImageList(UINT nBmpResourceID,UINT nWidth,UINT nHeight)
{
	CBitmap tmpBmp;
	if(tmpBmp.LoadBitmap(nBmpResourceID))
	{
		m_imglistDisable.DeleteImageList();

		BITMAP bmp;
		tmpBmp.GetBitmap(&bmp);
		if(m_imglistDisable.Create(nWidth,nHeight,ILD_IMAGE|ILD_TRANSPARENT,1, (int)(bmp.bmWidth/nWidth)))
		{
			m_imglistDisable.Add(&tmpBmp,RGB(0,0,0));
			CImageList *pPreImgList =  GetToolBarCtrl().SetDisabledImageList(&m_imglistDisable);
			if ( pPreImgList != NULL && pPreImgList != &m_imglistDisable)
			{
				pPreImgList->DeleteImageList();
			}
			return true;
		}
	}
	return false;
}

bool CEasyToolBar::SetButtonSpace(UINT nSpaceSize)
{
	TBMETRICS tm;
	tm.cbSize = sizeof(tm);
	tm.dwMask = TBMF_BUTTONSPACING;
	tm.cxButtonSpacing = nSpaceSize;
	tm.cyButtonSpacing = nSpaceSize;

	GetToolBarCtrl().SetMetrics(&tm);

	return true;
}

bool CEasyToolBar::SetButtonPad(UINT nPadWidth,UINT nPadHeigh)
{
	TBMETRICS tm;
	tm.cbSize = sizeof(tm);
	tm.dwMask = TBMF_PAD;
	tm.cxPad = nPadWidth;
	tm.cyPad = nPadHeigh;

	GetToolBarCtrl().SetMetrics(&tm);
	return true;
}

void CEasyToolBar::SetButtonBorder(bool bHaveBorder)
{
	m_bHaveBorder = bHaveBorder;
}
void CEasyToolBar::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW  lplvcd = (LPNMCUSTOMDRAW)pNMHDR;
	switch (lplvcd->dwDrawStage)
	{
	case CDDS_ITEMPREPAINT:
		{
			*pResult = (m_bHaveBorder?0:TBCDRF_NOEDGES|TBCDRF_NOBACKGROUND/*|TBCDRF_HILITEHOTTRACK*/)|TBCDRF_NOOFFSET;
			return ;
		}
		break;
	case CDDS_PREPAINT:
		{
			*pResult = CDRF_NOTIFYITEMDRAW;
			return ;
		}
		break;
	}
	*pResult = 0;
}
