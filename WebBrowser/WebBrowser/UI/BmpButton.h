#pragma once


// C58ImageButton
#include <atlimage.h>
class CBmpButton : public CButton
{
	DECLARE_DYNAMIC(CBmpButton)
public:
	CBmpButton();
	virtual ~CBmpButton();

protected:
	DECLARE_MESSAGE_MAP()
protected:
	CBitmap  m_bmpNormal;
	CBitmap  m_bmpHover;
	CBitmap  m_bmpPress;
	CBitmap  m_bmpDisable;

	HDC      m_memDC;

	byte   m_CurrentStatus;
	int   m_HoverStep;
	int   m_PressStep;
	BOOL  _bMouseTrack;
	CToolTipCtrl m_ToolTip;
	BOOL  m_bEnableWindow;

	HWND  m_hWndPressNotify;
	UINT  m_MsgPressNotify;
	
	byte m_bgRed;
	byte m_bgGreen;
	byte m_bgBlud;

public:
	bool LoadButtonImages(UINT nNormal,UINT nHover,UINT nPress,UINT nDisable);
	afx_msg void OnPaint();
	
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnEnable(BOOL bEnable);

	bool RegistMouseDownNotify(HWND hWnd, UINT nMessage);
	bool SetBGColor(byte Red, byte Green, byte Bule);
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};


