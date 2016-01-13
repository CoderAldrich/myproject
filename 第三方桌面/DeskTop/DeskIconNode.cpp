#include "StdAfx.h"
#include "DeskIconNode.h"

const int nZoomSize = 2;

CDeskIconNode::CDeskIconNode(void)
{
	m_bHover = FALSE;

	m_nWidth = 0;
	m_nHeight = 0;

	m_bDraging = FALSE;
}

CDeskIconNode::~CDeskIconNode(void)
{
}

VOID CDeskIconNode::ChangeText(LPCWSTR pszText)
{
	m_strText = pszText;
}

VOID CDeskIconNode::ChangePostion(int nNewX,int nNewY)
{
	m_rcBorder.MoveToX(nNewX);
	m_rcBorder.MoveToY(nNewY);
}
VOID CDeskIconNode::ChangeSize(int nNewWidth,int nNewHeight)
{
	m_rcBorder.right = m_rcBorder.left+nNewWidth;
	m_rcBorder.bottom = m_rcBorder.top+nNewHeight;
}
VOID CDeskIconNode::SetIcon(HICON hIcon)
{
	m_Icon.SetDeskIcon(hIcon);
}
VOID CDeskIconNode::GetIconRect(CRect *prcIcon)
{
	if (prcIcon)
	{
		*prcIcon = m_rcBorder;
	}
}
VOID CDeskIconNode::SetTargetFilePath(LPCWSTR pszFilePath)
{
	m_strFilePath = pszFilePath;
}
CString CDeskIconNode::GetTargetFilePath()
{
	return m_strFilePath;
}

BOOL CDeskIconNode::CheckMouseIn(int nX,int nY)
{
	CPoint ptMouse;
	ptMouse.SetPoint(nX,nY);
	if (PtInRect(&m_rcBorder,ptMouse))
	{
		return TRUE;
	}

	return FALSE;
}

VOID CDeskIconNode::SetDraging(BOOL bDraging,int nX,int nY)
{
	m_bDraging = bDraging;
	
	m_ptDragPoint.x = nX-m_rcBorder.left;
	m_ptDragPoint.y = nY-m_rcBorder.top;
	
}
BOOL CDeskIconNode::GetDraging(int &nOffsetX,int &nOffsetY)
{
	nOffsetX = m_ptDragPoint.x;
	nOffsetY = m_ptDragPoint.y;
	return m_bDraging;
}

VOID CDeskIconNode::SetSize(int nWidth,int nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
}
VOID CDeskIconNode::GetSize(int &nWidth,int &nHeight)
{
	nWidth = m_nWidth;
	nHeight = m_nHeight;
}
VOID CDeskIconNode::Draw(HDC hDC,int nX,int nY)
{
	CDC dc;
	dc.Attach(hDC);
	if(m_bHover)
	{

		CRect rcTemp;
		rcTemp = m_rcBorder;
		rcTemp.InflateRect(nZoomSize,nZoomSize,nZoomSize,nZoomSize);
		//dc.DrawEdge(&rcTemp,EDGE_SUNKEN,BF_RECT);
		::DrawIconEx(dc.m_hDC,rcTemp.left,rcTemp.top,m_Icon.GetDeskIcon(),rcTemp.Width(),rcTemp.Height(),0,NULL,DI_NORMAL);
	}
	else
	{
		::DrawIconEx(dc.m_hDC,m_rcBorder.left,m_rcBorder.top,m_Icon.GetDeskIcon(),m_rcBorder.Width(),m_rcBorder.Height(),0,NULL,DI_NORMAL);
	}

	dc.SelectObject((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	dc.SetTextColor(RGB(250,250,250));

	::SetBkMode(dc.m_hDC,TRANSPARENT);
	CRect rcText;
	rcText = m_rcBorder;
	rcText.top = rcText.bottom+5;
	if (m_bHover)
		rcText.top+=nZoomSize;
	rcText.bottom = rcText.top+40;
	rcText.left-=10;
	rcText.right+=10;

	dc.DrawTextEx(m_strText,rcText,DT_CENTER|DT_TOP|DT_WORDBREAK|DT_END_ELLIPSIS,NULL);
	dc.Detach();

}
VOID CDeskIconNode::OnMouseMoveHover( int nX,int nY )
{
	m_bHover = TRUE;
}
VOID CDeskIconNode::OnMouseMoveLeave()
{
	m_bHover = FALSE;
}
VOID CDeskIconNode::OnMouseLButtonDown( int nX,int nY )
{

}
VOID CDeskIconNode::OnMouseLButtonUp( int nX,int nY )
{

}
VOID CDeskIconNode::OnMouseLButtonDbClick( int nX,int nY )
{

}
VOID CDeskIconNode::OnMouseRButtonDown( int nX,int nY )
{

}
VOID CDeskIconNode::OnMouseRButtonUp( int nX,int nY )
{

}
VOID CDeskIconNode::OnMouseRButtonDbClick( int nX,int nY )
{

}