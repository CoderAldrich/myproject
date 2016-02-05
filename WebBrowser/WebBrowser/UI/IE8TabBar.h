#pragma once

#include "BmpButton.h"
#include "IE8NaviBar.h"
#include "IconDownLoad.h"

typedef struct ItemData
{
	PVOID PageID;
	HICON hIcon;
	CString strUrl;
}ItemData,*PItemData;

class CIETabBar : public CTabCtrl
{
	DECLARE_DYNAMIC(CIETabBar)

private:

	CBitmap m_bmpTabNormal;
	CBitmap m_bmpTabHot;

	CBitmap m_bmpClsBtNormal;
	CBitmap m_bmpClsBtPress;
	CBitmap m_bmpClsBtHover;

	//CBitmapButton m_btnClose;
	CBmpButton  m_btnClose;
	CPen      m_Pen;
	CRect  m_rcAddTab;
	HICON m_icoAddTab;
	int   m_nAddTabStatus;
	bool m_bWatching;
	CSurfaceWnd m_wndAddTabSurface;

	HCURSOR  m_hDragCursor;

	HDC     m_hMemDC;
	
	CIconDownLoad m_IconLoader;

	UINT maxWidht;
	UINT minWidht;
	UINT tabrightOffset;
	UINT textLeft;
	UINT textRight;
	UINT textTop;
	UINT textBottom;
	UINT m_nBottomHeight;

	CFont  m_defaultfont;

	CString strTempPath;
public:
	CIETabBar(UINT nBottomHeight,UINT nMaxWidth,UINT nMinWidht);
	virtual ~CIETabBar();
	HICON  m_hDefaultIcon;
	
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	void DrawTabNode(CDC *pDC,CRect rcItem,bool bHotItem,CString strItemText, HICON hIcon);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	LRESULT OnCloseBtnClick(WPARAM wParam,LPARAM lParam);
	LRESULT OnAddTabBtnClick(WPARAM wParam,LPARAM lParam);
	LRESULT OnIconChange(WPARAM wParam,LPARAM lParam);
	void AdjustTabWidth(void);
	BOOL AddTabItem(PVOID PageID,CString strPageUrl,CString strPageTitle);
	PVOID RemoveTabItem(PVOID PageID);
	BOOL ChangeFocusTabItem(PVOID PageID);
	BOOL UpdateTabItemUrl(PVOID PageID,CString strPageUrl);
	BOOL UpdateTabItemTitle(PVOID PageID,CString strTitle);
	BOOL UpdateTabItemIcon(PVOID PageID,HICON hIcon);

	CString GetTabItemTitle(PVOID PageID);
	CString GetTabItemUrl(PVOID PageID);
	void DrawTabBar(CDC *pDC);
	BOOL SetItemData(int Index,PVOID pData);
	PVOID GetItemData(int Index);
	int PageIDToIndex(PVOID PageID);
	PVOID IndexToPageID(int nIndex);
	void DrawAddNode(CDC * pDC, int nStatus);
	afx_msg void OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchanging(NMHDR *pNMHDR, LRESULT *pResult);
	BOOL SetNextSel();
	BOOL SetPreSel();
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnCLoseTab(int nIndex);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	bool m_bMouseDown;
	CPoint m_ptMouseDown;
};


