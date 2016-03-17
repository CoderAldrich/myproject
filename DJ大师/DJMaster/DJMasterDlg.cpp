
// DJMasterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DJMaster.h"
#include "DJMasterDlg.h"
#include "AddMusicDlg.h"
#include "HelpFun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



CDJMasterDlg::CDJMasterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDJMasterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bEnableEdit = TRUE;
}

void CDJMasterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDJMasterDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CDJMasterDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDJMasterDlg::OnBnClickedCancel)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON1, &CDJMasterDlg::OnBnClickedButton1)
	ON_WM_SIZE()
	ON_MESSAGE(WM_USER+2222,OnMusicChange)
	ON_MESSAGE(WM_USER+3333,OnVolumeChange)
END_MESSAGE_MAP()


// CDJMasterDlg 消息处理程序

BOOL CDJMasterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	CMenu *pMainMenu = new CMenu;
	pMainMenu->LoadMenu(IDR_MAIN_MENU);
	SetMenu(pMainMenu);

	m_wndMusicDisplay.Create(NULL,NULL,WS_VISIBLE|WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_TABSTOP,CRect(10,10,200,500),this,0);
	m_wndVolumeCtrl.Create(NULL,NULL,WS_VISIBLE|WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_TABSTOP,CRect(10,10,200,500),this,0);
	
	m_wndMusicDisplay.SetNotifyParam(m_hWnd,WM_USER+2222);
	m_wndVolumeCtrl.SetNotifyParam(m_hWnd,WM_USER+3333);
	
	CRect rcClient;
	GetClientRect(&rcClient);
	RelayoutChild(rcClient.Width(),rcClient.Height());
	
	m_wndVolumeCtrl.InitStatus(100);

//  	m_Mci.Open(L"D:\\搜狗高速下载\\Right Here Waiting.mp3");
//  	m_Mci.SetVolume(1000);
// 
//  	m_Mci.Play();
// 
// 	Sleep(5000);
// 
// 	m_Mci.Close();
// 	m_Mci.Open(L"D:\\搜狗高速下载\\410010151200128.mp3");
// 	m_Mci.Play();
// 	
// 	m_wndVolumeCtrl.SetRange(0,1000);
// 	m_wndVolumeCtrl.SetPos(1000);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDJMasterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDJMasterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDJMasterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDJMasterDlg::OnBnClickedOk()
{
	
}

void CDJMasterDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CDJMasterDlg::OnDropFiles(HDROP hDropInfo)
{
	UINT unFileCount;            
	WCHAR szFilePath[200];              
	unFileCount = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);   //系统的API函数         
	for( UINT i=0;i < unFileCount;i++)             
	{  
		int pathLen = DragQueryFileW(hDropInfo, i, szFilePath, _countof(szFilePath));  //API函数 

		AddMusicFile(szFilePath);

	}
	DragFinish(hDropInfo);   //API函数

	CDialog::OnDropFiles(hDropInfo);
}

void CDJMasterDlg::OnBnClickedButton1()
{
	if (m_bEnableEdit)
	{
		GetDlgItem(IDC_BUTTON1)->SetWindowText(L"启用编辑");
	}
	else
	{
		GetDlgItem(IDC_BUTTON1)->SetWindowText(L"禁止编辑");
	}
	
	m_wndMusicDisplay.LockEdit(m_bEnableEdit);
	m_bEnableEdit=!m_bEnableEdit;
}

void CDJMasterDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	RelayoutChild(cx,cy);
}
LRESULT CDJMasterDlg::OnMusicChange(WPARAM wParam,LPARAM lParam)
{
	PMUSIC_NOTE pNode = (PMUSIC_NOTE)wParam;
	m_Mci.Pause();
	m_Mci.Close();
	m_Mci.Open(pNode->strFilePath);
	m_Mci.Play();
	return 0;
}
LRESULT CDJMasterDlg::OnVolumeChange(WPARAM wParam,LPARAM lParam)
{
	m_Mci.SetVolume(wParam);
	return 0;
}
void CDJMasterDlg::RelayoutChild(int cx, int cy)
{
	if ( ::IsWindow(m_wndMusicDisplay.m_hWnd))
	{
		m_wndMusicDisplay.MoveWindow(10,10,300,cy-20);
	}

	if ( ::IsWindow(m_wndVolumeCtrl.m_hWnd))
	{
		m_wndVolumeCtrl.MoveWindow(cx-60,0,60,200);
	}
}

BOOL WINAPI FileFindCallBack( LPCWSTR pszFileFullPath , PVOID pParam)
{
	CDJMasterDlg *pThis = (CDJMasterDlg*)pParam;
	if (pThis)
	{
		pThis->AddMusicFile(pszFileFullPath);
	}

	return TRUE;
}

BOOL WINAPI DirectFindCallBack( LPCWSTR pszDirFullPath , PVOID pParam )
{
	return TRUE;
}

BOOL CDJMasterDlg::AddMusicFile(LPCWSTR pszFilePath)
{
	if(GetFileAttributesW(pszFilePath)&FILE_ATTRIBUTE_DIRECTORY)
	{
		FindPath(pszFilePath,L"*.*",FileFindCallBack,this,DirectFindCallBack,this);
	}
	else
	{
		CAddMusicDlg addDlg;
		addDlg.InitDisplay(pszFilePath);
		if ( IDOK == addDlg.DoModal() )
		{
			CString strMusicDesc;
			strMusicDesc = addDlg.GetMusicDescription();

			PMUSIC_NOTE pNode = new MUSIC_NOTE;
			pNode->strFilePath = pszFilePath;
			m_wndMusicDisplay.AddMusic(pNode,strMusicDesc);
		}
	}

	return 0;
}
