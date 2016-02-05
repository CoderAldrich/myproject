#include "stdafx.h"
#include "IEComboBoxEx.h"

CIEComboBoxEx::CIEComboBoxEx()
{

}
CIEComboBoxEx::~CIEComboBoxEx()
{

}BEGIN_MESSAGE_MAP(CIEComboBoxEx, CComboBoxEx)
ON_WM_CREATE()
ON_CONTROL_REFLECT(CBN_SELCHANGE, &CIEComboBoxEx::OnCbnSelchange)
END_MESSAGE_MAP()

int CIEComboBoxEx::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CComboBoxEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	comboexlist.Create(16, 16, ILC_COLOR32|ILC_MASK, 100, 100);
	comboexlist.Add(LoadIconW(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ADDR_BAR_DEFAULT)));
	SetImageList(&comboexlist);


	m_Edit.SubclassWindow(GetEditCtrl()->GetSafeHwnd());

 	GetEditCtrl()->ModifyStyle(0,ES_WANTRETURN,0);
 	UINT dwStyle = GetWindowLong(GetEditCtrl()->m_hWnd,GWL_STYLE);
 	dwStyle |= ES_WANTRETURN|ES_MULTILINE;
 	SetWindowLong(GetEditCtrl()->m_hWnd,GWL_STYLE,dwStyle);

	

	return 0;
}

bool CIEComboBoxEx::AddOrChoiceUrlItem(CString strUrl, CString strIcoFillPath)
{
	if( strUrl.GetLength() == 0 )
	{
		return false;
	}

	int nIndex = FindString(strUrl);
	if (nIndex < 0)
	{
		HICON hIcon = (HICON)LoadImageW(AfxGetInstanceHandle(),strIcoFillPath,IMAGE_ICON,16,16,LR_LOADFROMFILE);
		int nImageIndex = -1;
		if ( hIcon )
		{
			nImageIndex=comboexlist.Add(hIcon);
		}
		
		nImageIndex = nImageIndex >= 0 ?nImageIndex:0;

		CString strTemp;
		strTemp=strUrl;
		
		int   nItemCount = GetCount();

		COMBOBOXEXITEM     cbi ={0};
		cbi.mask = CBEIF_IMAGE | CBEIF_INDENT | CBEIF_OVERLAY | CBEIF_SELECTEDIMAGE | CBEIF_TEXT;
		cbi.iItem = nItemCount;
		cbi.pszText = strTemp.GetBuffer();
		cbi.cchTextMax = strTemp.GetLength();
		cbi.iImage = nImageIndex;
		cbi.iSelectedImage = nImageIndex;
		cbi.iOverlay = 2;
		cbi.iIndent = 0;

		InsertItem(&cbi);

		SetCurSel(nItemCount);
	}
	else
	{
		SetCurSel(nIndex);
	}

	return true;
}

bool CIEComboBoxEx::UpdateItemIcon(CString strUrl,CString strIcoFillPath)
{
	int nIndex = FindString(strUrl);
	
	if (nIndex >= 0)
	{
		HICON hIcon = (HICON)LoadImageW(AfxGetInstanceHandle(),strIcoFillPath,IMAGE_ICON,16,16,LR_LOADFROMFILE);
		if ( hIcon )
		{
			int nImageIndex = comboexlist.Add(hIcon);
			
			if ( nImageIndex > 0 )
			{
				CString strTemp;
				strTemp=strUrl;

				int   nItemCount = GetCount();

				COMBOBOXEXITEM     cbi ={0};
				cbi.mask = CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
				cbi.iItem = nIndex;
				cbi.iImage = nImageIndex;
				cbi.iSelectedImage = nImageIndex;
				SetItem(&cbi);
			}
		}
	}


	return true;
}

int CIEComboBoxEx::FindString(LPCTSTR lpszText)
{
	int  nIndex = -1;
	int   nItemCount = GetCount();

	CString strText;
	strText = lpszText;
	strText.MakeLower();
		
	for (int i=0;i<nItemCount;i++)
	{
		WCHAR szText[2000];
		COMBOBOXEXITEM     cbi ={0};
		cbi.mask = CBEIF_TEXT;
		cbi.iItem = i;
		cbi.pszText = szText;
		cbi.cchTextMax = 2000;
		if(GetItem(&cbi))
		{
			if (CString(szText).MakeLower() == strText)
			{
				nIndex = i;
				break;
			}
		}
	}
	return nIndex;
}
void CIEComboBoxEx::OnCbnSelchange()
{
	GetParentFrame()->SendMessage(WM_COMMAND,ID_GOTO_URL,0);
}

BOOL CIEComboBoxEx::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_MOUSEWHEEL)
	{
		return true;
	}
	return CComboBoxEx::PreTranslateMessage(pMsg);
}
