#pragma once

class CDeskIconNode;

#include <list>
using namespace std;

typedef list<CDeskIconNode *> LIST_DESK_ICON;
typedef LIST_DESK_ICON::iterator LIST_DESK_ICON_PTR;



class CDeskIconManager
{
protected:
	HWND m_wndShow;
	LIST_DESK_ICON m_listDeskIcon;
	
	UINT m_nIconSize;
	UINT m_nIconPadSize;
	CPoint  m_ptMouse;

	CImage m_imgBackGround;

	HDC m_hDC;
	CDeskIconNode *m_pPreFocusNode;
public:
	CDeskIconManager(void);
	~CDeskIconManager(void);

public:
	BOOL SetShowWnd(HWND hWndShow);
	VOID Draw(HDC hDC);
	BOOL AddIcon(LPCWSTR pszIconText,LPCWSTR pszIconFile);
	BOOL AddIcon(LPCWSTR pszIconText,HICON   hIcon, LPCWSTR pszTargetFilePath);

	VOID MousePosChange(int nX,int nY);
	VOID MouseDbClick(int nX,int nY);
	VOID MouseLButtonDown(int nX,int nY);
	VOID MouseLButtonUp(int nX,int nY);
	VOID MouseRClick(int nX,int nY);
};
