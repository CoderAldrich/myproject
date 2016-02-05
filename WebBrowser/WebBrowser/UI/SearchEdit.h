#pragma once


// CSearchEdit

class CSearchEdit : public CEdit
{
	DECLARE_DYNAMIC(CSearchEdit)
private:
	UINT m_msgReturnToParent;
public:
	CSearchEdit();
	virtual ~CSearchEdit();
	void SetNotifyMessageOnReturn(UINT nMsg);
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnSetFocus(CWnd* pOldWnd);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


