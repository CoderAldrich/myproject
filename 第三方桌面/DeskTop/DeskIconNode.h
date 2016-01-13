#pragma once

#include "DeskIcon.h"
#include "DeskNodeBase.h"
class CDeskIconNode:public CDeskNodeBase
{
protected:
	CString m_strText;

	CString m_strFilePath;
	CRect   m_rcBorder;
	BOOL    m_bHover;

	CDeskIcon m_Icon;

	int   m_nWidth;
	int   m_nHeight;

	BOOL  m_bDraging;
	CPoint  m_ptDragPoint;
public:
	CDeskIconNode(void);
	~CDeskIconNode(void);
	VOID ChangeText(LPCWSTR pszText);
	VOID ChangePostion(int nNewX,int nNewY);
	VOID ChangeSize(int nNewWidth,int nNewHeight);
	VOID SetIcon(HICON hIcon);
	VOID GetIconRect(CRect *prcIcon);
	VOID SetTargetFilePath(LPCWSTR pszFilePath);
	CString GetTargetFilePath();

	BOOL CheckMouseIn(int nX,int nY);
	VOID SetDraging(BOOL bDraging,int nX,int nY);
	BOOL GetDraging(int &nOffsetX,int &nOffsetY);


	VOID SetSize(int nWidth,int nHeight);
	VOID GetSize(int &nWidth,int &nHeight);
	VOID Draw(HDC hDC,int nX,int nY);
	VOID OnMouseMoveHover( int nX,int nY );
	VOID OnMouseMoveLeave();
	VOID OnMouseLButtonDown( int nX,int nY );
	VOID OnMouseLButtonUp( int nX,int nY );
	VOID OnMouseLButtonDbClick( int nX,int nY );
	VOID OnMouseRButtonDown( int nX,int nY );
	VOID OnMouseRButtonUp( int nX,int nY );
	VOID OnMouseRButtonDbClick( int nX,int nY );

};
