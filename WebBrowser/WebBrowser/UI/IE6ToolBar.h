#include "EasyToolBar.h"
class CIE6ToolBar:public CEasyToolBar 
{
public:
	CIE6ToolBar();
	virtual ~CIE6ToolBar();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};