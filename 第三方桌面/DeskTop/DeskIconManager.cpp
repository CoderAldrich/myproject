#include "StdAfx.h"
#include "DeskIconManager.h"
#include "DeskIconNode.h"

CDeskIconManager::CDeskIconManager(void)
{
	WCHAR szLocalPath[MAX_PATH]={0};
	GetModuleFileNameW(NULL,szLocalPath,MAX_PATH);
	WCHAR *pPathEnd = (WCHAR *)szLocalPath+wcslen(szLocalPath);
	while (pPathEnd != szLocalPath && *pPathEnd != L'\\') pPathEnd--;
	*(pPathEnd+1) = 0;

	wcscat_s(szLocalPath,L"002.jpg");

	m_imgBackGround.Load(szLocalPath);

	m_nIconSize = 50;
	m_nIconPadSize = 80;

	m_ptMouse.SetPoint(0,0);

	m_hDC = NULL;

	m_pPreFocusNode = NULL;
}

CDeskIconManager::~CDeskIconManager(void)
{

}

BOOL CDeskIconManager::SetShowWnd(HWND hWndShow)
{
	if (m_hDC && m_wndShow)
	{
		::ReleaseDC(m_wndShow,m_hDC);
	}

	m_wndShow = hWndShow;
	//m_hDC = ::GetDC(m_wndShow);

	return TRUE;
}

VOID CDeskIconManager::Draw(HDC hDC)
{
	if(hDC)
	{
		
		CRect rcWin;
		::GetClientRect(m_wndShow,&rcWin);

		HDC hMemDC = ::CreateCompatibleDC(hDC);
		HBITMAP  hMemBmp = ::CreateCompatibleBitmap(hDC,rcWin.Width(),rcWin.Height());

		::SelectObject(hMemDC,hMemBmp);

		::SetStretchBltMode(hMemDC,COLORONCOLOR);
		BOOL bRes = m_imgBackGround.Draw(hMemDC,0,0,rcWin.Width(),rcWin.Height());

 		for (LIST_DESK_ICON_PTR it = m_listDeskIcon.begin();it!=m_listDeskIcon.end();it++)
 		{
			CDeskNodeBase *pNode = (*it);
			int nWidth = 0;
			int nHeight = 0;
			pNode->GetSize(nWidth,nHeight);
 			pNode->Draw(hMemDC,nWidth,nHeight/*(*it)->GetMouseIn()*/);
 		}

		bRes = ::BitBlt(hDC,0,0,rcWin.Width(),rcWin.Height(),hMemDC,0,0,SRCCOPY);

		::DeleteObject(hMemBmp);
		::DeleteDC(hMemDC);

	}

}
BOOL CDeskIconManager::AddIcon(LPCWSTR pszIconText,LPCWSTR pszIconFile)
{
	CDeskIconNode *pNode = new CDeskIconNode;
	pNode->ChangeText(pszIconText);

	CRect rcWin;
	::GetWindowRect(m_wndShow,&rcWin);

	const int nMaxLineCount = rcWin.Height()/m_nIconPadSize;
	const int nIconCount = m_listDeskIcon.size();
	const int nOffset = (m_nIconPadSize-m_nIconSize)/2;

	pNode->ChangePostion( nOffset+nIconCount/nMaxLineCount*m_nIconPadSize,nOffset+nIconCount%nMaxLineCount *m_nIconPadSize);

	pNode->ChangeSize(m_nIconSize,m_nIconSize);

	m_listDeskIcon.push_back(pNode);
	return FALSE;
}

BOOL CDeskIconManager::AddIcon(LPCWSTR pszIconText,HICON   hIcon , LPCWSTR pszTargetFilePath)
{
	CDeskIconNode *pNode = new CDeskIconNode;
	pNode->ChangeText(pszIconText);

	CRect rcWin;
	::GetWindowRect(m_wndShow,&rcWin);

	const int nMaxLineCount = rcWin.Height()/m_nIconPadSize;
	const int nIconCount = m_listDeskIcon.size();
	const int nOffset = (m_nIconPadSize-m_nIconSize)/2;

	pNode->ChangePostion( nOffset+nIconCount/nMaxLineCount*m_nIconPadSize,nOffset+nIconCount%nMaxLineCount *m_nIconPadSize);

	pNode->ChangeSize(m_nIconSize,m_nIconSize);

	pNode->SetIcon(hIcon);
	pNode->SetTargetFilePath(pszTargetFilePath);

	m_listDeskIcon.push_back(pNode);

	return FALSE;
}

VOID CDeskIconManager::MousePosChange(int nX,int nY)
{
	m_ptMouse.SetPoint(nX,nY);
	
	CDeskIconNode *pFocusNode = NULL;

	for (LIST_DESK_ICON_PTR it = m_listDeskIcon.begin();it!=m_listDeskIcon.end();it++)
	{
		if((*it)->CheckMouseIn(nX,nY))
		{
			(*it)->OnMouseMoveHover( nX,nY );
			//(*it)->ChangePostion(nX-30,nY-30);
			pFocusNode = (*it);
		}
		else
		{
			(*it)->OnMouseMoveLeave();
		}

		int nOffsetX,nOffsetY;
		if ((*it)->GetDraging(nOffsetX,nOffsetY))
		{
			CRect rcIcon;
			(*it)->GetIconRect(&rcIcon);
			
			(*it)->ChangePostion(nX-nOffsetX,nY-nOffsetY);
				
			
			rcIcon.InflateRect(50,50,50,50);
			::InvalidateRect(m_wndShow,&rcIcon,TRUE);

		}
	}

	if ( pFocusNode != m_pPreFocusNode )
	{
		if (m_pPreFocusNode)
		{
			m_pPreFocusNode->OnMouseMoveLeave();

			CRect rcIcon;
			m_pPreFocusNode->GetIconRect(&rcIcon);
			rcIcon.InflateRect(50,50,50,50);
			::InvalidateRect(m_wndShow,&rcIcon,TRUE);
		}

		if (pFocusNode)
		{
			pFocusNode->OnMouseMoveHover( nX,nY );
			CRect rcIcon;
			pFocusNode->GetIconRect(&rcIcon);
			rcIcon.InflateRect(50,50,50,50);
			::InvalidateRect(m_wndShow,&rcIcon,TRUE);
		}
	}

	m_pPreFocusNode = pFocusNode;


}

VOID CDeskIconManager::MouseDbClick(int nX,int nY)
{
	for (LIST_DESK_ICON_PTR it = m_listDeskIcon.begin();it!=m_listDeskIcon.end();it++)
	{
		if((*it)->CheckMouseIn(nX,nY))
		{
			ShellExecuteW(m_wndShow,L"open",(*it)->GetTargetFilePath(),NULL,NULL,SW_SHOW);
			break;
		}
	}
}

VOID CDeskIconManager::MouseLButtonDown(int nX,int nY)
{
	for (LIST_DESK_ICON_PTR it = m_listDeskIcon.begin();it!=m_listDeskIcon.end();it++)
	{
		if((*it)->CheckMouseIn(nX,nY))
		{
			(*it)->SetDraging(TRUE,nX,nY);
			break;
		}
	}
}
VOID CDeskIconManager::MouseLButtonUp(int nX,int nY)
{
	for (LIST_DESK_ICON_PTR it = m_listDeskIcon.begin();it!=m_listDeskIcon.end();it++)
	{
		(*it)->SetDraging(FALSE,0,0);
	}
}

VOID CDeskIconManager::MouseRClick(int nX,int nY)
{
	//个性化
	//rundll32.exe shell32.dll,Control_RunDLL desk.cpl,1,2
	
	HMODULE hShellMod = LoadLibrary(L"shell32.dll");

	if (hShellMod)
	{
		typedef void ( WINAPI *TypeRunDll32FunW)(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow);

		TypeRunDll32FunW pFun = (TypeRunDll32FunW)::GetProcAddress(hShellMod,"Control_RunDLLW");
		if (pFun)
		{
			pFun(m_wndShow,NULL,L"desk.cpl,1,2",SW_SHOW);
		}
	}


 	CMenu Menu;
 	Menu.CreatePopupMenu();
 	Menu.AppendMenu(MF_STRING,0,L"刷新(&E)");
	Menu.AppendMenu(MF_STRING,0,L"个性化(&R)");
	//Menu.AppendMenu(MF_STRING,0,L"个性化(&R)");
 	::TrackPopupMenu(Menu.m_hMenu,TPM_LEFTALIGN|TPM_TOPALIGN,nX,nY,0,m_wndShow,NULL);
	
}