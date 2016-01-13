#pragma once

class CDeskNodeBase
{
public:
	virtual VOID SetSize(int nWidth,int nHeight) = 0;
	virtual VOID GetSize(int &nWidth,int &nHeight) = 0;
	virtual VOID Draw(HDC hDC,int nX,int nY) = 0;
	virtual VOID OnMouseMoveHover( int nX,int nY ) = 0;
	virtual VOID OnMouseMoveLeave() = 0;
	virtual VOID OnMouseLButtonDown( int nX,int nY ) = 0;
	virtual VOID OnMouseLButtonUp( int nX,int nY ) = 0;
	virtual VOID OnMouseLButtonDbClick( int nX,int nY ) = 0;
	virtual VOID OnMouseRButtonDown( int nX,int nY ) = 0;
	virtual VOID OnMouseRButtonUp( int nX,int nY ) = 0;
	virtual VOID OnMouseRButtonDbClick( int nX,int nY ) = 0;

};
