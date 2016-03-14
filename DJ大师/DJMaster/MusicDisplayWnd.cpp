// MusicDisplayWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "DJMaster.h"
#include "MusicDisplayWnd.h"


// CMusicDisplayWnd

IMPLEMENT_DYNAMIC(CMusicDisplayWnd, CWnd)

CMusicDisplayWnd::CMusicDisplayWnd()
{
	m_bDragingButton = FALSE;
	m_bLockEdit = FALSE;

	m_nBtnSelIndex = -1;
	m_nDragTaggetIndexPre = -1;
	m_nDragTaggetIndexNext = -1;
}

CMusicDisplayWnd::~CMusicDisplayWnd()
{
}


BEGIN_MESSAGE_MAP(CMusicDisplayWnd, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()



BOOL CMusicDisplayWnd::AddMusic( LPCWSTR pszFilePath,LPCWSTR pszMusicDesc )
{
	const int nBtnHeight = 30;
	CRect rcClient;
	GetClientRect(&rcClient);

	BUTTON_INFO Info;
	Info.nIndex = m_lstButtons.size();
	Info.strFilePath = pszFilePath;
	Info.strMusicDesc = pszMusicDesc;

	m_lstButtons.push_back(Info);

	InvalidateRect(&rcClient);

	return FALSE;
}

VOID CMusicDisplayWnd::DelMusic( int nIndex)
{
	if (nIndex < 0 || nIndex > m_lstButtons.size())
	{
		return;
	}

	LIST_MUSIC_BUTTON tmpListButtons;
	for (LIST_MUSIC_BUTTON_PTR it = m_lstButtons.begin();it!=m_lstButtons.end();it++)
	{
		if (it->nIndex != nIndex )
		{
			if( it->nIndex > nIndex )
			{
				it->nIndex--;
			}
			tmpListButtons.push_back(*it);
		}
	}

	m_lstButtons.clear();

	for (LIST_MUSIC_BUTTON_PTR it = tmpListButtons.begin();it!=tmpListButtons.end();it++)
	{
		m_lstButtons.push_back(*it);
	}

	this->RedrawWindow();
}

VOID CMusicDisplayWnd::CalcBtnRect(int nIndex,CRect &rcBtn)
{
	CRect rcClient;
	GetClientRect(&rcClient);

	rcBtn.left = 0;
	rcBtn.top = nIndex*30;
	rcBtn.right = rcClient.Width();
	rcBtn.bottom = (nIndex+1)*30;
	rcBtn.DeflateRect(2,2,2,2);
}

VOID CMusicDisplayWnd::MoveButton(int nIndex,int nTargetIndexPre,int nTargetIndexNext)
{

	if ( nIndex < 0 || nIndex > m_lstButtons.size()|| nIndex == nTargetIndexPre || nIndex == nTargetIndexNext  || nTargetIndexNext - nTargetIndexPre != 1 )
	{
		return ;
	}
	
	//向前移动
	if ( nIndex > nTargetIndexNext )
	{
		for (LIST_MUSIC_BUTTON_PTR it = m_lstButtons.begin();it!=m_lstButtons.end();it++)
		{
			if (it->nIndex == nIndex )
			{
				it->nIndex = nTargetIndexNext;
			}
			else if (it->nIndex > nTargetIndexPre && it->nIndex < nIndex )
			{
				it->nIndex++;
			}
		}
	}
	//向后移动
	else if( nIndex < nTargetIndexPre )
	{
		for (LIST_MUSIC_BUTTON_PTR it = m_lstButtons.begin();it!=m_lstButtons.end();it++)
		{
			if (it->nIndex == nIndex )
			{
				it->nIndex = nTargetIndexPre;
			}
			else if (it->nIndex > nIndex && it->nIndex < nTargetIndexNext )
			{
				it->nIndex--;
			}
		}
	}

}

int  CMusicDisplayWnd::PointToIndex( int nX,int nY )
{
	int nIndex = -1;
	for (LIST_MUSIC_BUTTON_PTR it = m_lstButtons.begin();it!=m_lstButtons.end();it++)
	{
		CRect rcBtn;
		CPoint ptCheck;
		CalcBtnRect(it->nIndex,rcBtn);
		ptCheck.SetPoint(nX,nY);
		if (PtInRect(&rcBtn,ptCheck))
		{
			nIndex = it->nIndex;
			break;
		}
	}

	return nIndex;
}
int CMusicDisplayWnd::PointToIndex( LPPOINT pptCheck )
{
	return PointToIndex(pptCheck->x,pptCheck->y);
}

CMusicDisplayWnd::LIST_MUSIC_BUTTON_PTR CMusicDisplayWnd::IndexToIterator(int nIndex)
{
	LIST_MUSIC_BUTTON_PTR it = m_lstButtons.begin();
	for (;it!=m_lstButtons.end() ;it++)
	{
		if ( nIndex == it->nIndex)
		{
			break;
		}
	}

	return it;
}

BOOL CMusicDisplayWnd::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(&rcClient);

	pDC->Rectangle(rcClient);

	return TRUE;
}

void CMusicDisplayWnd::OnPaint()
{
	CPaintDC dc(this);

	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(GetStockObject(DEFAULT_GUI_FONT));

	for (LIST_MUSIC_BUTTON_PTR it = m_lstButtons.begin();it!=m_lstButtons.end();it++)
	{
		CRect rcBtn;
		CalcBtnRect(it->nIndex,rcBtn);
		dc.Rectangle(rcBtn);
		dc.DrawTextEx(it->strMusicDesc,rcBtn,DT_CENTER|DT_VCENTER|DT_SINGLELINE,0);

		CString strIndex;
		strIndex.Format(L"%d",it->nIndex+1);
		rcBtn.right = rcBtn.Height();
		dc.DrawTextEx(strIndex,rcBtn,DT_CENTER|DT_VCENTER|DT_SINGLELINE,0);
	}
	
	if ( m_bDragingButton )
	{
		CRect rcBtn;
		CalcBtnRect(m_nDragTaggetIndexNext,rcBtn);

		rcBtn.top-=5;
		rcBtn.bottom = rcBtn.top+5;

		dc.FillSolidRect(rcBtn,RGB(0,0,0));

	}
}

void CMusicDisplayWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_nBtnSelIndex = PointToIndex(&point);
	m_nDragTaggetIndexPre = -1;
	m_nDragTaggetIndexNext = -1;

	CWnd::OnLButtonDown(nFlags, point);
}

void CMusicDisplayWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if ( m_nBtnSelIndex >=0 && m_nBtnSelIndex< m_lstButtons.size() )
	{
		//拖拽后释放鼠标
		if( m_bDragingButton )
		{
			MoveButton(m_nBtnSelIndex,m_nDragTaggetIndexPre,m_nDragTaggetIndexNext);
		}
		else //点击了按钮
		{
			int nIndexClick = PointToIndex(&point);
			if ( nIndexClick != -1 && nIndexClick == m_nBtnSelIndex )
			{
				LIST_MUSIC_BUTTON_PTR it = IndexToIterator(nIndexClick);
				AfxMessageBox(it->strFilePath + L"  " + it->strMusicDesc);
			}

		}

		m_bDragingButton = FALSE;
		m_nBtnSelIndex = -1;
		m_nDragTaggetIndexPre = -1;
		m_nDragTaggetIndexNext = -1;

		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidateRect(&rcClient);
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CMusicDisplayWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( m_bLockEdit )
	{
		return;
	}
	if ( m_nBtnSelIndex >= 0 && m_nBtnSelIndex < m_lstButtons.size() )
	{
		m_bDragingButton = TRUE;
	}

	if ( m_bDragingButton )
	{
		CRect rcClient;
		GetClientRect(&rcClient);

		int nBtnCount = m_lstButtons.size();
		for (int i=-1;i<nBtnCount;i++)
		{
			CRect rcPreBtn;
			CRect rcNextBtn;
			CalcBtnRect(i,rcPreBtn);
			CalcBtnRect(i+1,rcNextBtn);

			if (point.y <= rcNextBtn.CenterPoint().y && point.y >= rcPreBtn.CenterPoint().y)
			{
				if (m_nDragTaggetIndexPre != i || m_nDragTaggetIndexNext != i+1 )
				{
					m_nDragTaggetIndexPre = i;
					m_nDragTaggetIndexNext = i+1;

					InvalidateRect(&rcClient);
				}

				break;
			}

		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CMusicDisplayWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	if( FALSE == m_bLockEdit )
	{
		int nIndexClick = PointToIndex(&point);
		if ( nIndexClick >= 0 )
		{
			CPoint ptCurMouse;
			GetCursorPos(&ptCurMouse);
			CMenu menu;
			menu.LoadMenu(IDR_MENU1);
			int nMenuCmd = menu.GetSubMenu(0)->TrackPopupMenu(TPM_RETURNCMD,ptCurMouse.x,ptCurMouse.y,this);
			if (ID_ROOT_32771 == nMenuCmd )
			{
				DelMusic(nIndexClick);
			}
		}
		
	}
	CWnd::OnRButtonUp(nFlags, point);
}
