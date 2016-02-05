// 58ImageButton.cpp : 实现文件
//

#include "stdafx.h"
#include "BmpButton.h"

IMPLEMENT_DYNAMIC(CBmpButton, CButton)
CBmpButton::CBmpButton()
{
	m_CurrentStatus=0;//0 normal 1 hover 2 press 3 disable
	m_HoverStep = 0;;
	m_PressStep = 0;
	m_bEnableWindow=true;
	m_memDC = CreateCompatibleDC(NULL);
	m_hWndPressNotify = NULL;
	m_MsgPressNotify = 0;

	m_bgRed = 0;
	m_bgGreen = 0;
	m_bgBlud = 0;
}

CBmpButton::~CBmpButton()
{
	DeleteDC(m_memDC);

}


BEGIN_MESSAGE_MAP(CBmpButton, CButton)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_ENABLE()
END_MESSAGE_MAP()



// C58ImageButton 消息处理程序

bool CBmpButton::LoadButtonImages(UINT nNormal,UINT nHover,UINT nPress,UINT nDisable)
{
	if(nNormal) m_bmpNormal.LoadBitmap(nNormal);
	if(nHover) m_bmpHover.LoadBitmap(nHover);
	if(nPress) m_bmpPress.LoadBitmap(nPress);
	if(nDisable) m_bmpDisable.LoadBitmap(nDisable);

	return true;
}

UINT GetBmpWidth(CBitmap &cbmp)
{
	BITMAP bmp;
	cbmp.GetBitmap(&bmp);
	return bmp.bmWidth;
}

UINT GetBmpHeight(CBitmap &cbmp)
{
	BITMAP bmp;
	cbmp.GetBitmap(&bmp);
	return bmp.bmHeight;
}
void CBmpButton::OnPaint()
{
	CPaintDC dc(this); 

	CRect winrect;
	this->GetClientRect(&winrect);

	int width=winrect.Width();
	int height=winrect.Height();

	if (m_bEnableWindow)
	{
		if (m_CurrentStatus == 0)
		{
			::SelectObject(m_memDC,m_bmpNormal);
			::TransparentBlt(dc.m_hDC,0,0,winrect.Width(),winrect.Height(),m_memDC,0,0,GetBmpWidth(m_bmpNormal),GetBmpHeight(m_bmpNormal),RGB(m_bgRed,m_bgGreen,m_bgBlud));
		}
		else if(m_CurrentStatus == 1 )
		{
			::SelectObject(m_memDC,m_bmpHover);
			::TransparentBlt(dc.m_hDC,0,0,winrect.Width(),winrect.Height(),m_memDC,0,0,GetBmpWidth(m_bmpHover),GetBmpHeight(m_bmpHover),RGB(m_bgRed,m_bgGreen,m_bgBlud));

		}
		else if(m_CurrentStatus == 2)
		{
			::SelectObject(m_memDC,m_bmpPress);
			::TransparentBlt(dc.m_hDC,0,0,winrect.Width(),winrect.Height(),m_memDC,0,0,GetBmpWidth(m_bmpPress),GetBmpHeight(m_bmpPress),RGB(m_bgRed,m_bgGreen,m_bgBlud));
		}
	}
	else
	{
		::SelectObject(m_memDC,m_bmpDisable);
		::TransparentBlt(dc.m_hDC,0,0,winrect.Width(),winrect.Height(),m_memDC,0,0,GetBmpWidth(m_bmpDisable),GetBmpHeight(m_bmpDisable),RGB(m_bgRed,m_bgGreen,m_bgBlud));

	}
}

void CBmpButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (_bMouseTrack)
	{
		TRACKMOUSEEVENT csTME;
		csTME.cbSize = sizeof(csTME);
		csTME.dwFlags = TME_LEAVE|TME_HOVER;
		csTME.hwndTrack = m_hWnd;//指定要追踪的窗口
		csTME.dwHoverTime = 10;  //鼠标在按钮上停留超过10ms，才认为状态为HOVER
		::_TrackMouseEvent(&csTME); //开启Windows的WM_MOUSELEAVE，WM_MOUSEHOVER事件支持
		_bMouseTrack=FALSE;   //若已经追踪，则停止追踪
	}
	CButton::OnMouseMove(nFlags, point);
}
void CBmpButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	
	m_CurrentStatus = 2;

	CRect winrect;
	this->GetWindowRect(&winrect);
	GetParent()->ScreenToClient(&winrect);
	GetParent()->InvalidateRect(&winrect);

	CRect clientrect;
	this->GetClientRect(&clientrect);
	this->InvalidateRect(&clientrect,TRUE);

	CButton::OnLButtonDown(nFlags, point);

}

void CBmpButton::OnLButtonUp(UINT nFlags, CPoint point)
{
    CRect winrect;
    this->GetWindowRect(&winrect);
    GetParent()->ScreenToClient(&winrect);
    GetParent()->InvalidateRect(&winrect);

    CRect clientrect;
    this->GetClientRect(&clientrect);
    this->InvalidateRect(&clientrect,TRUE);

    if(point.x > clientrect.BottomRight().x || point.y > clientrect.BottomRight().y)
    {
        // 鼠标松开右键时 已移出按钮区域
        m_CurrentStatus = 0;
    }
    else if(m_CurrentStatus==2)
    {
        m_CurrentStatus = 1;

        if(m_hWndPressNotify)
        {
            ::SendMessage(m_hWndPressNotify,m_MsgPressNotify,0,0);
        }
    }
	CButton::OnLButtonUp(nFlags, point);
}

BOOL CBmpButton::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg);	

	return CButton::PreTranslateMessage(pMsg);
}

int CBmpButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (m_ToolTip.m_hWnd == NULL)
	{
		m_ToolTip.Create(this);
		m_ToolTip.Activate(TRUE);
	}

	CRect rectBtn; 
	CString wintitle;
	GetClientRect(rectBtn);
	this->GetWindowText(wintitle);
	m_ToolTip.AddTool(this, wintitle, rectBtn, 1);
	m_ToolTip.UpdateTipText(wintitle, this, 1);
	m_ToolTip.Activate(true);

	return 0;
}

// BOOL CBmpButton::CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, LPVOID lpParam)
// {
// 	return CButton::CreateEx(dwExStyle, lpszClassName, lpszWindowName, dwStyle|WS_CLIPSIBLINGS, rect, pParentWnd, nID, lpParam);
// }

// BOOL CBmpButton::Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
// {
// 	return CButton::Create(lpszCaption, dwStyle|WS_CLIPSIBLINGS, rect, pParentWnd, nID);
// }

BOOL CBmpButton::OnEraseBkgnd(CDC* pDC)
{
	//CPoint pt;
	//CRect rcClip;
	//pDC->GetClipBox(rcClip);
	//pt = rcClip.TopLeft();
	//MapWindowPoints(GetParent(), &pt, 1);
	//CPoint orgPt = pDC->OffsetWindowOrg(pt.x, pt.y);

	//GetParent()->SendMessage(WM_ERASEBKGND,(WPARAM)(pDC->m_hDC),0);

	//pDC->SetWindowOrg(orgPt.x, orgPt.y);

	return true;
}

void CBmpButton::OnMouseHover(UINT nFlags, CPoint point)
{
	m_CurrentStatus = 1;

	CRect winrect;
	this->GetWindowRect(&winrect);
	GetParent()->ScreenToClient(&winrect);
	GetParent()->InvalidateRect(&winrect);

	CRect clientrect;
	this->GetClientRect(&clientrect);
	this->InvalidateRect(&clientrect,TRUE);
	//
// 	GetParent()->RedrawWindow();
// 	this->RedrawWindow();


	CButton::OnMouseHover(nFlags, point);
}

void CBmpButton::OnMouseLeave()
{
	KillTimer(101);
	m_HoverStep=0;
	m_CurrentStatus = 0;
	CRect winrect;
	this->GetWindowRect(&winrect);
	GetParent()->ScreenToClient(&winrect);
	GetParent()->InvalidateRect(&winrect);

	CRect clientrect;
	this->GetClientRect(&clientrect);
	this->InvalidateRect(&clientrect,TRUE);

	_bMouseTrack=TRUE;


	CButton::OnMouseLeave();
}

void CBmpButton::OnEnable(BOOL bEnable)
{
	//CButton::OnEnable(bEnable);
	m_bEnableWindow=bEnable;
	CRect winrect;
	this->GetWindowRect(&winrect);
	GetParent()->ScreenToClient(&winrect);
	GetParent()->InvalidateRect(&winrect);

	CRect clientrect;
	this->GetClientRect(&clientrect);
	this->InvalidateRect(&clientrect,TRUE);
	//this->RedrawWindow();
	//Invalidate(TRUE);
}


bool CBmpButton::RegistMouseDownNotify(HWND hWnd, UINT nMessage)
{

	m_hWndPressNotify = hWnd;
	m_MsgPressNotify = nMessage;

	return true;
}

bool CBmpButton::SetBGColor(byte Red, byte Green, byte Bule)
{
	m_bgRed = Red;
	m_bgGreen = Green;
	m_bgBlud = Bule;
	return true;
}

void CBmpButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if(lpDrawItemStruct->CtlType == ODT_BUTTON)
	{
		int a=0;
	}
}

BOOL CBmpButton::PreCreateWindow(CREATESTRUCT& cs)
{
	//cs.dwExStyle|=WS_EX_TRANSPARENT;
	return CButton::PreCreateWindow(cs);
}
