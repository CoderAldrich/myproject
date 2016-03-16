#pragma once


// CVolumeCtrl

class CVolumeCtrl : public CWnd
{
	DECLARE_DYNAMIC(CVolumeCtrl)

public:
	CVolumeCtrl();
	virtual ~CVolumeCtrl();
protected:
	UINT  m_uPos;
	UINT  m_uMinValue;
	UINT  m_uMaxValue;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	VOID CalcCtrlBlockRect(CRect & rcCtrlBlock);
	VOID SetValueRange(UINT uMinValue, UINT uMaxValue);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


