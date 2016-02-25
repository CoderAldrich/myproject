// IE8TabBar.cpp : 实现文件
//

#include "stdafx.h"
#include "UIPublic.h"
#include "IE8TabBar.h"
#include "BrowserPublic.h"
#include "..\resource.h"
// CIE8TabBar



IMPLEMENT_DYNAMIC(CIETabBar, CTabCtrl)

CIETabBar::CIETabBar(UINT nBottomHeight,UINT nMaxWidth,UINT nMinWidht)
: m_bMouseDown(false)
{
	maxWidht = nMaxWidth;
	minWidht = nMinWidht;
	m_nBottomHeight = nBottomHeight;


	//#define IDB_TAB_BASE_RIGHT              311
	//#define IDB_TAB_HOT_BASE_RIGHT          313

	if(GetIsWindowBaseTheme())
	{
		m_nBottomHeight = 0;
		m_bmpTabNormal.LoadBitmap(IDB_TAB_BASE);
		m_bmpTabHot.LoadBitmap(IDB_TAB_HOT_BASE);
	}
	else
	{
		m_bmpTabNormal.LoadBitmap(IDB_TAB_NORMAL);
		m_bmpTabHot.LoadBitmap(IDB_TAB_HOT);
	}

	m_bmpClsBtNormal.LoadBitmap(IDB_TAB_BTN_NORMAL);
	m_bmpClsBtPress.LoadBitmap(IDB_TAB_BTN_PRESS);
	m_bmpClsBtHover.LoadBitmap(IDB_TAB_BTN_HOVER);
	m_hMemDC = CreateCompatibleDC(NULL);

	tabrightOffset = 70;
	textLeft = 30;
	textRight = 30;
	textTop = 6;
	textBottom = 0;
	m_defaultfont.Attach((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	m_hDefaultIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_IE8_TAB_DEFAULT));
	m_icoAddTab = NULL;//LoadIconW(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_IE9_ADD_TAB));
	m_nAddTabStatus = 0;
	m_bWatching = false;
	m_Pen.CreatePen(PS_SOLID,1,RGB(163,167,172));

	GetTempPathW(MAX_PATH,strTempPath.GetBuffer(MAX_PATH));
	strTempPath.ReleaseBuffer();
}

CIETabBar::~CIETabBar()
{
	DeleteDC(m_hMemDC);
}


BEGIN_MESSAGE_MAP(CIETabBar, CTabCtrl)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_MESSAGE(WM_TAB_BTN_CLOSE,OnCloseBtnClick)
	ON_MESSAGE(WM_TAB_BTN_ADD,OnAddTabBtnClick)
	ON_MESSAGE(WM_TAB_NOTIFY_ICON_CHANGE,OnIconChange)

	ON_NOTIFY_REFLECT(TCN_SELCHANGE, &CIETabBar::OnTcnSelchange)
	ON_NOTIFY_REFLECT(TCN_SELCHANGING, &CIETabBar::OnTcnSelchanging)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

LRESULT CIETabBar::OnCloseBtnClick(WPARAM wParam,LPARAM lParam)
{
	int index = GetCurSel();

	if (index >= 0)
	{
		OnCLoseTab(index);
	}

	return 0;
}
void CIETabBar::OnCLoseTab(int nIndex)
{
	PAGEID nPageIDRemoved = (PAGEID)IndexToPageID(nIndex);

 	CFrameWnd *pParentFrame = GetParentFrame();
	if (pParentFrame)
	{
		pParentFrame->PostMessage(	WM_TAB_CLOSE,nPageIDRemoved,0);
	}
}

LRESULT CIETabBar::OnAddTabBtnClick(WPARAM wParam,LPARAM lParam)
{
	if (wParam == 1)
	{
		m_nAddTabStatus = 0;
		InvalidateRect(&m_rcAddTab,TRUE);
	}
	else if( wParam == 2)
	{
		m_nAddTabStatus = 1;
		InvalidateRect(&m_rcAddTab,TRUE);
	}
	else if(wParam == 3)
	{
		::PostMessageW(GetParentFrame()->m_hWnd,WM_COMMAND,ID_NEW_TAB,0);
	}
	return 0;
}
LRESULT CIETabBar::OnIconChange(WPARAM wParam,LPARAM lParam)
{
	WCHAR *pszFilePath = (WCHAR *)lParam;
	PVOID PageID = (PVOID)wParam;

	UpdateTabItemIcon(
		PageID,
		(HICON)LoadImageW(AfxGetInstanceHandle(),pszFilePath,IMAGE_ICON,16,16,LR_LOADFROMFILE)
		);
	delete pszFilePath;

	return 0;
}

int CIETabBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTabCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_btnClose.Create(TEXT("关闭这个标签"),WS_VISIBLE|WS_CHILD/*|BS_OWNERDRAW*/,CRect(0,0,17,16),this,0);

	m_btnClose.LoadButtonImages(IDB_TAB_BTN_NORMAL,IDB_TAB_BTN_HOVER,IDB_TAB_BTN_PRESS,0);
	m_btnClose.SetBGColor(255,255,255);

	m_btnClose.RegistMouseDownNotify(m_hWnd,WM_TAB_BTN_CLOSE);

	m_wndAddTabSurface.Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CRect(0,0,30,30),this,0);
	m_wndAddTabSurface.RegisterStatusChangeNotify(m_hWnd,WM_TAB_BTN_ADD);

	return 0;
}

void CIETabBar::DrawTabBar(CDC *pDC)
{

	CRect rcWin;
	this->GetWindowRect(&rcWin);
	ScreenToClient(&rcWin);


	HDC hPaintDC = CreateCompatibleDC( pDC->m_hDC );
	HBITMAP hPaintBmp = CreateCompatibleBitmap( pDC->m_hDC ,rcWin.Width(),rcWin.Height());

	::SelectObject(hPaintDC,hPaintBmp);

	CDC dc;
	dc.Attach(hPaintDC);
	

 	CPoint pt;
 	pt.SetPoint(0,0);
 	MapWindowPoints(GetParent(), &pt, 1);
 	CPoint orgPt = dc.OffsetWindowOrg(pt.x,pt.y);
 	GetParent()->SendMessage(WM_ERASEBKGND,(WPARAM)(dc.m_hDC),0);
 	dc.SetWindowOrg(orgPt.x, orgPt.y);

	UINT nItemCount = GetItemCount();
	UINT nHotItem = GetCurSel();
	for (UINT i=0;i<nItemCount;i++)
	{
		//跳过获取热点的Tab最后画
		if (nHotItem == i)
		{
			continue;
		}

		CRect rcItem;
		GetItemRect(i,&rcItem);

		WCHAR titlebuffer[2048]={0};
		TCITEM iteminfo;
		iteminfo.mask = TCIF_TEXT;
		iteminfo.pszText=titlebuffer;
		iteminfo.cchTextMax=2048;
		BOOL res=GetItem(i,&iteminfo);


		HICON hIcon = m_hDefaultIcon;
		ItemData *pData = (ItemData *)GetItemData(i);
		if (pData && pData->hIcon)
		{
			hIcon = pData->hIcon;
		}
		DrawTabNode(&dc,rcItem,false,titlebuffer,hIcon);
	}

	//最后画获取焦点的项
	CRect rcItem;
	GetItemRect(nHotItem,&rcItem);

	WCHAR titlebuffer[2048]={0};
	TCITEM iteminfo;
	iteminfo.mask = TCIF_TEXT;
	iteminfo.pszText=titlebuffer;
	iteminfo.cchTextMax=2048;
	BOOL res=GetItem(nHotItem,&iteminfo);

	HICON hIcon = m_hDefaultIcon;
	ItemData *pData = (ItemData *)GetItemData(nHotItem);
	if (pData && pData->hIcon)
	{
		hIcon = pData->hIcon;
	}
	DrawTabNode(&dc,rcItem,true,titlebuffer,hIcon);


	//移动关闭按钮
 	CRect rcHotItem;
 	GetItemRect(GetCurSel(),&rcHotItem);
 	CRect rcBtn;
 	m_btnClose.GetWindowRect(&rcBtn);
 	rcHotItem.top+=4;
 	rcHotItem.bottom=rcHotItem.top+rcBtn.Height();
 	rcHotItem.right-=4;
 	rcHotItem.left = rcHotItem.right-rcBtn.Width();
 	m_btnClose.MoveWindow(rcHotItem/*,FALSE*/);

	//移动添加标签按钮的Surface
	GetItemRect(GetItemCount()-1,&m_rcAddTab);
	m_rcAddTab.left = m_rcAddTab.right;
	m_rcAddTab.right+=30;
	m_wndAddTabSurface.MoveWindow(&m_rcAddTab);

	//画添加标签按钮
	DrawAddNode(&dc,m_nAddTabStatus);

	if(m_nBottomHeight)
	{
		//画底栏
		UINT nTopOffset = rcWin.Height()-m_nBottomHeight;
		dc.FillSolidRect(0,nTopOffset,rcWin.Width(),m_nBottomHeight,RGB(227,238,251));
		dc.SelectObject(m_Pen);
		dc.MoveTo(0,nTopOffset);
		dc.LineTo(rcItem.left,nTopOffset);

		dc.MoveTo(rcItem.right,nTopOffset);
		dc.LineTo(rcWin.Width(),nTopOffset);
	}

	pDC->BitBlt(0,0,rcWin.Width(),rcWin.Height(),&dc,0,0,SRCCOPY);

	dc.Detach();

	DeleteDC(hPaintDC);
	DeleteObject(hPaintBmp);


}

void CIETabBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	DrawTabBar(&dc);
}

BOOL CIETabBar::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CIETabBar::DrawTabNode(CDC *pDC,CRect rcItem,bool bHotItem,CString strItemText, HICON hIcon)
{
	static const UINT nNotHotLow = 1;
	CRect orgItemRect;
	orgItemRect = rcItem;

	UINT nTopOffSet = 0;
	if (bHotItem)
	{
		nTopOffSet = 0;
		::SelectObject(m_hMemDC,m_bmpTabHot);
	}
	else
	{
		nTopOffSet = nNotHotLow;
		::SelectObject(m_hMemDC,m_bmpTabNormal);
	}
	
	BITMAP tmpBmp;
	m_bmpTabNormal.GetBitmap(&tmpBmp);

	::StretchBlt(
		pDC->m_hDC,rcItem.left,nTopOffSet,3,rcItem.Height(),
		m_hMemDC,  0,0,3,tmpBmp.bmHeight,
		SRCCOPY
		);
	::StretchBlt(
		pDC->m_hDC,rcItem.left+3,nTopOffSet,rcItem.Width()-3*2,rcItem.Height(),
		m_hMemDC,  3,0,3,tmpBmp.bmHeight,
		SRCCOPY
		);
	::StretchBlt(
		pDC->m_hDC,rcItem.right-3,nTopOffSet,3,rcItem.Height(),
		m_hMemDC,  3*2,0,3,tmpBmp.bmHeight,
		SRCCOPY
		);

	::SetBkMode(pDC->m_hDC,TRANSPARENT);
	rcItem.left+=textLeft;
	rcItem.top+=textTop;
	rcItem.right-=textRight;
	::SelectObject(pDC->m_hDC,m_defaultfont);
	if (strItemText.GetLength() > 0)
	{
		DrawText(pDC->m_hDC,strItemText.GetBuffer(),strItemText.GetLength(),&rcItem,DT_LEFT|DT_TOP|DT_SINGLELINE|DT_WORD_ELLIPSIS);
	}
	
	if (hIcon)
	{
		int nleft= (orgItemRect.Height() - 16)/2+orgItemRect.left;
		int ntop = (orgItemRect.Height()-nTopOffSet - 16)/2+nTopOffSet;
		DrawIconEx(pDC->m_hDC,nleft,ntop,hIcon,16,16,0,NULL,DI_NORMAL);
	}

}
void CIETabBar::DrawAddNode(CDC * pDC, int nStatus)
{
	static int nIconSize = 16;
	DrawTabNode(pDC,m_rcAddTab,false,TEXT(""),NULL);
	if(nStatus == 1)
	{
		DrawIconEx(pDC->m_hDC,(m_rcAddTab.Width()-nIconSize)/2+m_rcAddTab.left,(m_rcAddTab.Height()-nIconSize)/2,m_icoAddTab,nIconSize,nIconSize,0,NULL,DI_NORMAL);
	}
}


void CIETabBar::OnSize(UINT nType, int cx, int cy)
{
	CTabCtrl::OnSize(nType,cx,cy);
	AdjustTabWidth();
}

void CIETabBar::AdjustTabWidth(void)
{
	CRect winrect;
	this->GetWindowRect(&winrect);
	int itemcount=this->GetItemCount();
	if (itemcount > 0)
	{
		UINT width=((float)(winrect.Width()-tabrightOffset))/((float)itemcount);

		if(width>=maxWidht)
		{
			width=maxWidht;
		}
		else if(width > minWidht)
		{
			width=(winrect.Width()-tabrightOffset)/itemcount;
		}else
		{
			width=minWidht;
		}

		SetItemSize(CSize(width,winrect.Height()));

	}
}

BOOL CIETabBar::AddTabItem(PVOID PageID,CString strPageUrl,CString strPageTitle)
{
	if ( PageID == NULL )
	{
		return false;
	}

	int nInsertIndex = GetCurSel()+1;
	LONG nNewIndex = InsertItem(nInsertIndex,strPageTitle.GetLength() == 0?strPageUrl:strPageTitle);
	
	bool bRes = false;
	if (nNewIndex != -1)
	{
		if(strPageUrl.GetLength() > 0)
		{
			CString strIconFileName;
			strIconFileName = strTempPath+UrlToFaviconFileName(UrlToFaviconUrl(strPageUrl));
			if (::PathFileExistsW(strIconFileName))
			{
				UpdateTabItemIcon(
					PageID,
					(HICON)LoadImageW(AfxGetInstanceHandle(),strIconFileName,IMAGE_ICON,16,16,LR_LOADFROMFILE)
					);
			}
			else
			{
				WCHAR  *pszFilePath = new WCHAR[strIconFileName.GetLength()+1];
				wcscpy_s(pszFilePath,strIconFileName.GetLength()+1,strIconFileName.GetBuffer());
				m_IconLoader.StartDownload(m_hWnd,WM_TAB_NOTIFY_ICON_CHANGE,(WPARAM)PageID,(LPARAM)pszFilePath,UrlToFaviconUrl(strPageUrl),strIconFileName);
			}
		}

		ItemData *pData = new ItemData;
		pData->PageID = PageID;
		pData->hIcon = NULL;
		pData->strUrl = strPageUrl;

		if(SetItemData(nNewIndex,pData))
		{
			SetCurSel(nInsertIndex);
			bRes = true;
		}
		else
		{
			DeleteItem(nNewIndex);
			delete pData;

			bRes = false;;
		}
	}
	
	AdjustTabWidth();

	return bRes;

}	

PVOID CIETabBar::RemoveTabItem(PVOID PageID)
{
	BOOL bRes = false;
	PAGEID NextPageID = 0;
	int nCount = GetItemCount();
	int nIndex = PageIDToIndex(PageID);
	if ( nIndex >= 0 )
	{
		ItemData *pData = (ItemData *)GetItemData(nIndex);
		if (pData)
		{
			DestroyIcon(pData->hIcon);
			delete pData;
		}
		bRes = DeleteItem(nIndex);
		
		if (nCount > 1)
		{
			//此处给UI一定的自主权，决定显示那个Page

			UINT nNextIndex = 0;
			
			//如果关闭的是第一个Tab ，则他后一个Tab获取焦点
			if ( nIndex == 0 )
			{
				nNextIndex = 0;
			}
			else //关闭的不是第一个Tab ，则前一个Tab获取焦点
			{
				nNextIndex = nIndex-1;
			}

			ItemData *pTempData = (ItemData *)GetItemData(nNextIndex);
			if (pTempData)
			{
				NextPageID = (PAGEID)(pTempData->PageID);
			}

		}
// 		else
// 		{
// 			GetParentFrame()->CloseWindow();
// 			GetParentFrame()->PostMessage(WM_CLOSE,0,0);
// 		}
	}
	AdjustTabWidth();
	return (PVOID)NextPageID;
}
BOOL CIETabBar::ChangeFocusTabItem(PVOID PageID)
{
	BOOL bRes = false;
	int nIndex = PageIDToIndex(PageID);
	if ( nIndex >= 0 )
	{
		SetCurSel(nIndex);
		bRes = TRUE;
	}
	return bRes;
}
BOOL CIETabBar::UpdateTabItemUrl(PVOID PageID,CString strPageUrl)
{
	int nIndex = PageIDToIndex(PageID);
	if ( nIndex != -1 && strPageUrl.GetLength() > 0 )
	{
		ItemData *pData = (ItemData *)GetItemData(nIndex);
		if (pData)
		{
			if (pData->strUrl != strPageUrl)
			{
				pData->strUrl = strPageUrl;

				CString strIconFileName;
				strIconFileName = strTempPath+UrlToFaviconFileName(UrlToFaviconUrl(strPageUrl));
				if (::PathFileExistsW(strIconFileName))
				{
					UpdateTabItemIcon(
						PageID,
						(HICON)LoadImageW(AfxGetInstanceHandle(),strIconFileName,IMAGE_ICON,16,16,LR_LOADFROMFILE)
						);
				}
				else
				{
					WCHAR  *pszFilePath = new WCHAR[strIconFileName.GetLength()+1];
					wcscpy_s(pszFilePath,strIconFileName.GetLength()+1,strIconFileName.GetBuffer());
					m_IconLoader.StartDownload(m_hWnd,WM_TAB_NOTIFY_ICON_CHANGE,(WPARAM)PageID,(LPARAM)pszFilePath,UrlToFaviconUrl(strPageUrl),strIconFileName);
				}
			}
			return true;
		}
	}
	return false;
}


BOOL CIETabBar::UpdateTabItemTitle(PVOID PageID,CString strTitle)
{
	int nIndex = PageIDToIndex(PageID);
	if ( nIndex >= 0 )
	{
		TCITEM ti={0};
		ti.mask = TCIF_TEXT;
		ti.pszText = strTitle.GetBuffer();
		ti.cchTextMax = strTitle.GetLength();

		return SetItem(nIndex,&ti);
	}
	return FALSE;
}

BOOL CIETabBar::UpdateTabItemIcon(PVOID PageID,HICON hIcon)
{
	int nIndex = PageIDToIndex(PageID);
	if ( nIndex >= 0 )
	{
		ItemData *pData = (ItemData *)GetItemData(nIndex);
		if (pData)
		{
			if ( pData->hIcon != NULL && pData->hIcon != m_hDefaultIcon )
			{
				DestroyIcon(pData->hIcon);
			}

			pData->hIcon = hIcon;

			CRect rcItem;
			GetItemRect(nIndex,&rcItem);
			this->InvalidateRect(rcItem);

			return true;
		}
	}
	return false;
}
CString CIETabBar::GetTabItemTitle(PVOID PageID)
{
	int nIndex = PageIDToIndex(PageID);
	if ( nIndex >=0 )
	{
		WCHAR titlebuffer[2048]={0};
		TCITEM iteminfo;
		iteminfo.mask = TCIF_TEXT;
		iteminfo.pszText=titlebuffer;
		iteminfo.cchTextMax=2048;
		GetItem(nIndex,&iteminfo);
		return titlebuffer;
	}
	return TEXT("");
}
CString CIETabBar::GetTabItemUrl(PVOID PageID)
{
	PAGEID nPageID = PageIDToIndex(PageID);
	
	int nIndex = PageIDToIndex(PageID);
	if ( nIndex >=0 )
	{
		ItemData *pData = (ItemData *)GetItemData(nIndex);
		if (pData)
		{	
			return pData->strUrl;
		}
		
	}
	return TEXT("");
}
BOOL CIETabBar::SetItemData(int Index,PVOID pData)
{
	TCITEM ti={0};
	ti.mask = TCIF_PARAM;
	ti.lParam = (LPARAM)pData;
	return SetItem(Index,&ti);
}
PVOID CIETabBar::GetItemData(int Index)
{
	TCITEM ti={0};
	ti.mask = TCIF_PARAM;
	if(!GetItem(Index,&ti))
	{
		return NULL;
	}
	return (PVOID)(ti.lParam);
}
int CIETabBar::PageIDToIndex(PVOID PageID)
{
	int nIndex = -1;
	int nCount = GetItemCount();

	if ( PageID != NULL )
	{
		for (int i=0;i<nCount;i++)
		{
			PVOID pData = GetItemData(i);
			if ( pData != NULL && ((ItemData *)pData)->PageID == PageID)
			{
				nIndex = i;
				break;
			}
		}
	}
	return nIndex;
}
PVOID CIETabBar::IndexToPageID(int nIndex)
{
	ItemData *pData = (ItemData *)GetItemData(nIndex);
	if (pData)
	{
		return pData->PageID;
	}
	return NULL;
}
void CIETabBar::OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nIndex = GetCurSel();
	int nCount = GetItemCount();
	if (nIndex >= 0 && nIndex < nCount)
	{
		GetParentFrame()->PostMessage(WM_TAB_SEL_CHANGE,(PAGEID)IndexToPageID(nIndex),0);
	}
	*pResult = 0;
}


void CIETabBar::OnTcnSelchanging(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
BOOL CIETabBar::PreCreateWindow(CREATESTRUCT& cs)
{
	return CTabCtrl::PreCreateWindow(cs);
}

void CIETabBar::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	TCHITTESTINFO tchit;
	tchit.pt = point;
	int nIndex = this->HitTest(&tchit);
	OnCLoseTab(nIndex);
	CTabCtrl::OnLButtonDblClk(nFlags, point);
}

void CIETabBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bMouseDown = true;
	m_ptMouseDown = point;
	CTabCtrl::OnLButtonDown(nFlags, point);
}

void CIETabBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bMouseDown = false;
	CTabCtrl::OnLButtonUp(nFlags, point);
}

void CIETabBar::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bMouseDown)
	{
		if (
			abs(m_ptMouseDown.y - point.y) > 10
			)
		{
			m_bMouseDown = false;
			TCHITTESTINFO tchit;
			tchit.pt = point;
			int nIndex = this->HitTest(&tchit);
			ItemData *pData = (ItemData *)GetItemData(nIndex);
			GetParentFrame()->SendMessage(WM_TAB_DRAG,(WPARAM)pData->PageID,0);
		}

	}
	CTabCtrl::OnMouseMove(nFlags, point);
}

BOOL CIETabBar::SetNextSel()
{
	int nCount = GetItemCount();
	int nCurSel = GetCurSel();

	if (nCount >= 2)
	{
		if ( nCurSel == nCount-1 )//如果选择的是最后一个
		{
			SetCurSel(0);
		}
		else
		{
			SetCurSel(nCurSel + 1);
		}
	}
	return TRUE;
}
BOOL CIETabBar::SetPreSel()
{
	int nCount = GetItemCount();
	int nCurSel = GetCurSel();

	if (nCount >= 2)
	{
		if ( nCurSel == 0 )//如果选择的是第一个
		{
			SetCurSel(nCount-1);
		}
		else
		{
			SetCurSel(nCurSel - 1);
		}
	}
	return TRUE;
}