#pragma once


// CNoButtonBorderToolBar

class CEasyToolBar : public CToolBar
{
	DECLARE_DYNAMIC(CEasyToolBar)
private:
	CImageList    m_imglistNormal;
	CImageList    m_imglistHot;
	CImageList    m_imglistPress;
	CImageList    m_imglistDisable;
	bool          m_bHaveBorder;
public:
	CEasyToolBar();
	virtual ~CEasyToolBar();
	bool AddButton(UINT nIndex,UINT nCmdID = 0,BYTE ButtonStyle = TBBS_BUTTON|TBBS_AUTOSIZE,BYTE ButtnStatus = TBSTATE_ENABLED,int iBmpIndex = I_IMAGENONE,int iStringIndex = -1);
	bool SetImageList(UINT nBmpResourceID,UINT nWidth,UINT nHeight);
	bool SetHotImageList(UINT nBmpResourceID,UINT nWidth,UINT nHeight);
	bool SetPressImageList(UINT nBmpResourceID,UINT nWidth,UINT nHeight);
	bool SetDisableImageList(UINT nBmpResourceID,UINT nWidth,UINT nHeight);

	bool SetButtonSpace(UINT nSpaceSize);
	bool SetButtonPad( UINT nPadWidth,UINT nPadHeigh );
	void SetButtonBorder(bool bHaveBorder);
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};


