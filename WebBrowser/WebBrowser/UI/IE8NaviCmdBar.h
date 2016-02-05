#pragma once

#include "EasyToolBar.h"
class CIE8NaviCmdBar : public CEasyToolBar
{
	DECLARE_DYNAMIC(CIE8NaviCmdBar)
public:
	CIE8NaviCmdBar();
	virtual ~CIE8NaviCmdBar();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


