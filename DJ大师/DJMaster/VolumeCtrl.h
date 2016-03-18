#pragma once

#include <afxcmn.h>


class CVolumeCtrl : public CWnd
{
	DECLARE_DYNAMIC(CVolumeCtrl)
protected:
	UINT        m_uCurVolume;
	CSliderCtrl m_wndVolume;
	CStatic     m_wndStatic;

	HWND        m_hWndNotify;
	UINT        m_uMsgNotify;
public:
	CVolumeCtrl();
	virtual ~CVolumeCtrl();
protected:
	DECLARE_MESSAGE_MAP()

	VOID UpdateStaticShow();
	VOID UpdateSpliderShow();
	VOID NotifyChange();
public:
	VOID SetNotifyParam(HWND hWndNotify,UINT uMsgNotify);
	VOID InitStatus(UINT uInitVolume);
	UINT GetVolumeValue();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	VOID RelayoutChild(int nX, int nY);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};


