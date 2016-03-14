#pragma once

#include "MusicButton.h"

#include <list>

class CMusicDisplayWnd : public CWnd
{
private:
	BOOL  m_bDragingButton;
	BOOL  m_bLockEdit;
	int   m_nBtnSelIndex;
	int   m_nDragTaggetIndexPre;
	int   m_nDragTaggetIndexNext;

public:
	typedef struct tagBUTTON_INFO
	{
		int nIndex;
		CString strFilePath;
		CString strMusicDesc;
	}BUTTON_INFO,PBUTTON_INFO;
	typedef std::list<PBUTTON_INFO> LIST_MUSIC_BUTTON;
	typedef LIST_MUSIC_BUTTON::iterator LIST_MUSIC_BUTTON_PTR;

	DECLARE_DYNAMIC(CMusicDisplayWnd)
protected:
	LIST_MUSIC_BUTTON m_lstButtons;

	HDC  m_hMemDC;
	HBITMAP m_hMemBmp;

public:
	CMusicDisplayWnd();
	virtual ~CMusicDisplayWnd();

protected:
	DECLARE_MESSAGE_MAP()

public:
	BOOL AddMusic( LPCWSTR pszFilePath,LPCWSTR pszMusicDesc );
	VOID DelMusic( int nIndex);
	VOID CalcBtnRect(int nIndex,CRect &rcBtn);
	VOID MoveButton(int nIndex,int nTargetIndexPre,int nTargetIndexNext);
	int  PointToIndex( int nX,int nY );
	int  PointToIndex( LPPOINT pptCheck );
	LIST_MUSIC_BUTTON_PTR IndexToIterator(int nIndex);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	VOID ReCreateMemDCIfNeed(int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	VOID LockEdit(bool bLock);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};


