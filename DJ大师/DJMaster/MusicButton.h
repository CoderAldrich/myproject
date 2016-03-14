#pragma once


// CMusicButton

class CMusicButton : public CButton
{
	DECLARE_DYNAMIC(CMusicButton)

public:
	CMusicButton();
	virtual ~CMusicButton();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


