
// DeskTopDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DeskTop.h"
#include "DeskTopDlg.h"

#include "DeskIconManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CDeskTopDlg::CDeskTopDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDeskTopDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_hWndDesktop = NULL;
}

void CDeskTopDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDeskTopDlg, CDialog)
	
	ON_WM_PAINT()

	ON_BN_CLICKED(IDOK, &CDeskTopDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDeskTopDlg::OnBnClickedCancel)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND_RANGE(500,1000,OnMenuCmd)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_BUTTON1, &CDeskTopDlg::OnBnClickedButton1)
END_MESSAGE_MAP()

#include <shlobj.h>
#include <shlguid.h>
#include <shellapi.h>
#include <commctrl.h>
#include <commoncontrols.h>

HICON GetFileIcon(LPCWSTR pszFilePath)
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

		}
	}

	return hIcon;
}
// CDeskTopDlg 消息处理程序

BOOL CDeskTopDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	HWND hProgMan = ::FindWindow(L"ProgMan", NULL);
	
	if(hProgMan)
	{
		HWND hShellDefView = ::FindWindowEx(hProgMan, NULL, L"SHELLDLL_DefView", NULL);
		if(hShellDefView)
			m_hWndDesktop = ::FindWindowEx(hShellDefView, NULL, L"SysListView32", NULL);
	}

	::SetParent(m_hWnd,m_hWndDesktop);

	MoveWindow(0,0,GetSystemMetrics ( SM_CXSCREEN )	,GetSystemMetrics ( SM_CYSCREEN ));

	m_DeskIconManager.SetShowWnd(m_hWnd);


	CFileFind finder; 
	CString strPath; 
	BOOL bWorking = finder.FindFile(L"C:\\Users\\GaoZan\\Desktop\\*.*"); 
	while(bWorking) 
	{
		bWorking = finder.FindNextFile(); 

		strPath = finder.GetFilePath(); 
		OutputDebugStringW(strPath+L"\n");
		if(finder.IsDirectory() && !finder.IsDots()) 
		{
			int a=0;
		}
		else if(!finder.IsDirectory() && !finder.IsDots()) 
		{
			CString strIconText;
			strIconText = finder.GetFileTitle();

			CString strNotePadPath(strPath);
			SHFILEINFO stFileInfo;
			:: SHGetFileInfo( strNotePadPath,0,&stFileInfo,sizeof(stFileInfo),SHGFI_ICON);
			HICON hIcon = GetFileIcon(strPath);
			m_DeskIconManager.AddIcon(strIconText,/*stFileInfo.hIcon*/hIcon,strPath);
		}
	} 

	bWorking = finder.FindFile(L"C:\\Users\\Public\\Desktop\\*.*"); 
	while(bWorking) 
	{
		bWorking = finder.FindNextFile(); 

		strPath = finder.GetFilePath(); 
		OutputDebugStringW(strPath+L"\n");
		if(finder.IsDirectory() && !finder.IsDots()) 
		{
			int a=0;
		}
		else if(!finder.IsDirectory() && !finder.IsDots()) 
		{
			CString strIconText;
			strIconText = finder.GetFileTitle();

			CString strNotePadPath(strPath);
			SHFILEINFO stFileInfo;
			:: SHGetFileInfo( strNotePadPath,0,&stFileInfo,sizeof(stFileInfo),SHGFI_ICON);
			HICON hIcon = GetFileIcon(strPath);
			m_DeskIconManager.AddIcon(strIconText,/*stFileInfo.hIcon*/hIcon,strPath);
		}
	} 


// 	for (int i=0;i<10;i++)
// 	{
// 		CString strIconText;
// 		strIconText.Format(L"图标_%d",i);
// 
// 		CString strNotePadPath("C:\\Users\\GaoZan\\Desktop\\ADSafe.lnk");
// 		SHFILEINFO stFileInfo;
// 		:: SHGetFileInfo( strNotePadPath,0,&stFileInfo,sizeof(stFileInfo),SHGFI_ICON);
// 
// 		m_DeskIconManager.AddIcon(strIconText,stFileInfo.hIcon);
// 	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDeskTopDlg::OnPaint()
{
	CPaintDC DC(this);
	m_DeskIconManager.Draw(DC.m_hDC);
}

void CDeskTopDlg::OnBnClickedOk()
{

}

void CDeskTopDlg::OnBnClickedCancel()
{

}

BOOL CDeskTopDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CDeskTopDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	m_DeskIconManager.MousePosChange(point.x,point.y);

	CDialog::OnMouseMove(nFlags, point);
}

void CDeskTopDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	CMenu Menu;
	Menu.CreatePopupMenu();
	Menu.AppendMenuW(MF_STRING,600,L"刷新(&E)");
	Menu.AppendMenuW(MF_STRING,650,L"屏幕分辨率(&R)");
	Menu.AppendMenuW(MF_STRING,700,L"个性化(&R)");

	CMenu subMenu;
	subMenu.CreatePopupMenu();
	subMenu.AppendMenu(MF_STRING,800,L"新建文件夹(&F)");

	Menu.AppendMenu(MF_BYPOSITION|MF_POPUP|MF_STRING,(UINT_PTR)(subMenu.m_hMenu),L"新建(&W)");

	::TrackPopupMenu(Menu.m_hMenu,TPM_LEFTALIGN|TPM_TOPALIGN,point.x,point.y,0,m_hWnd,NULL);

	CDialog::OnRButtonUp(nFlags, point);
}
void CDeskTopDlg::OnMenuCmd(UINT nID)
{
	typedef void ( WINAPI *TypeRunDll32FunW)(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow);
	TypeRunDll32FunW pFun = NULL;

	HMODULE hShellMod = GetModuleHandleW(L"shell32.dll");
	if ( NULL == hShellMod )
	{
		hShellMod = ::LoadLibrary(L"shell32.dll");
	}
	if (hShellMod)
	{
		
		pFun = (TypeRunDll32FunW)::GetProcAddress(hShellMod,"Control_RunDLLW");
	}
	switch (nID)
	{
	case 600:
		{
			::SHChangeNotify( SHCNE_ASSOCCHANGED , SHCNF_IDLIST | SHCNF_FLUSH, 0, 0);  
		}
		break;
	case 650:
		{
			if (pFun)
			{
				pFun(m_hWnd,NULL,L"desk.cpl",SW_SHOW);
			}
		}
		break;
	case 700:
		{
			if (pFun)
			{
				pFun(m_hWnd,NULL,L"desk.cpl,1,2",SW_SHOW);
			}
		}
		break;

	case 800:
		{
			WCHAR path[255];
			SHGetSpecialFolderPath(0,path,CSIDL_DESKTOPDIRECTORY,0);
			wcscat(path,L"\\新建文件夹");

			CString strTemp;
			strTemp = path;

			int nNewIndex = 1;
			while ( TRUE == PathFileExists(strTemp) )
			{
				strTemp.Format(L"%s(%d)",path,nNewIndex);
			}

			CreateDirectory(strTemp,NULL);
		}
		break;
	}
}
void CDeskTopDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	
	m_DeskIconManager.MouseDbClick(point.x,point.y);
	CDialog::OnLButtonDblClk(nFlags, point);
}

void CDeskTopDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_DeskIconManager.MouseLButtonDown(point.x,point.y);
	CDialog::OnLButtonDown(nFlags, point);
}

void CDeskTopDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_DeskIconManager.MouseLButtonUp(point.x,point.y);
	CDialog::OnLButtonUp(nFlags, point);
}

void CDeskTopDlg::OnBnClickedButton1()
{
	this->ShowWindow(SW_HIDE);
}
