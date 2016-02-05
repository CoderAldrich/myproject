
#include "EasyToolBar.h"
#include "IEAddrEdit.h"
class CMyComboBox:public CComboBox
{
private:
	CBrush  m_dropBrush;
public:
	CMyComboBox()
	{

	}
	~CMyComboBox()
	{

	}
	void Draw(CDC *pDC);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnCbnSelchange();
	afx_msg void OnCbnSelendok();
public:
	bool AddOrChoiceItem(LPCTSTR pszUrl);
};

class CIE9AddrBar:public CControlBar
{
public:
	CIE9AddrBar();
	virtual ~CIE9AddrBar();
private:
	CIEAddrEdit  m_wndEdit;
	CMyComboBox  m_wndAddr;
	CEasyToolBar m_wndTravBar;
	CFont m_defaultfont;
	HICON  m_defaulticon;
public:
	DECLARE_MESSAGE_MAP()
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
	{
		
	}

	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	void RelayoutChildWindow(void);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnAddrFocus();
public:
	CComboBox *GetAddrPtr();
};