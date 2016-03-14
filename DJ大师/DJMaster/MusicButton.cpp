// MusicButton.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DJMaster.h"
#include "MusicButton.h"


// CMusicButton

IMPLEMENT_DYNAMIC(CMusicButton, CButton)

CMusicButton::CMusicButton()
{

}

CMusicButton::~CMusicButton()
{
}


BEGIN_MESSAGE_MAP(CMusicButton, CButton)
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CMusicButton ��Ϣ�������



int CMusicButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;

	CFont font;
	font.Attach(GetStockObject(DEFAULT_GUI_FONT));
	SetFont(&font,FALSE);

	return 0;
}
