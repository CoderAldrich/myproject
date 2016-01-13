#include "StdAfx.h"
#include "DeskIcon.h"

#include <shlobj.h>
#include <shlguid.h>
#include <shellapi.h>
#include <commctrl.h>
#include <commoncontrols.h>

CDeskIcon::CDeskIcon(void)
{
}

CDeskIcon::~CDeskIcon(void)
{
}

HICON CDeskIcon::GetFileIcon(LPCWSTR pszFilePath)
{
	SHFILEINFOW sfi = {0};
	SHGetFileInfo(pszFilePath, -1, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX);

	// Retrieve the system image list.
	// To get the 48x48 icons, use SHIL_EXTRALARGE
	// To get the 256x256 icons (Vista only), use SHIL_JUMBO
	HIMAGELIST* imageList;
	HRESULT hResult = SHGetImageList(SHIL_EXTRALARGE, IID_IImageList, (void**)&imageList);
	HICON hIcon;
	if (hResult == S_OK) {
		// Get the icon we need from the list. Note that the HIMAGELIST we retrieved
		// earlier needs to be casted to the IImageList interface before use.

		hResult = ((IImageList*)imageList)->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &hIcon);

		if (hResult == S_OK) {
			// Do something with the icon here.
			// For example, in wxWidgets:
			// 			wxIcon* icon = new wxIcon();
			// 			icon->SetHICON((WXHICON)hIcon);
			// 			icon->SetSize(48, 48);

			int a=0;
		}
	}

	return hIcon;
}

CDeskIcon * CDeskIcon::CreateDeskIcon( LPCWSTR pszTargetFile )
{
	CDeskIcon *pNewDIcon = new CDeskIcon;
	
	HICON hIcon = GetFileIcon(pszTargetFile);
	pNewDIcon->SetDeskIcon(hIcon);

	return pNewDIcon;
}

BOOL CDeskIcon::SetDeskIcon(HICON hIcon)
{
	if (m_hIcon)
	{
		DeleteObject(m_hIcon);
	}

	m_hIcon = hIcon;

	return TRUE;
}

HICON CDeskIcon::GetDeskIcon()
{
	return m_hIcon;
}