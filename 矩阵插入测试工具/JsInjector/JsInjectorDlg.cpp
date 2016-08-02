
// JsInjectorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "JsInjector.h"
#include "JsInjectorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL InstallEventHook( );
VOID UpdateJsText( LPCWSTR pszJsData , BOOL bJsUrl );

CJsInjectorDlg *g_pThis = NULL;
VOID MyOutputDebugStringW(LPCWSTR pszMsgOut)
{
	if (g_pThis)
	{
		int nLen = wcslen(pszMsgOut);
		WCHAR *pszData = new WCHAR[nLen+1];
		wcscpy_s(pszData,nLen+1,pszMsgOut);
		g_pThis->PostMessage(WM_USER+1122,(UINT)pszData,0);
	}
}

void InternetSettingModify()
{
	HKEY hKey = NULL;
	DWORD dwGetData = 0;
	DWORD dwType = REG_DWORD;
	DWORD cbData = sizeof(DWORD);

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER,
		_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),
		0,
		KEY_ALL_ACCESS | KEY_WOW64_64KEY,
		&hKey))
	{
		if (ERROR_SUCCESS ==RegQueryValueEx(hKey, _T("1609"), 0, &dwType, (PBYTE)&dwGetData, &cbData))
		{
			if (dwGetData != 0)
			{
				dwGetData = 0;
				RegSetValueEx(hKey, _T("1609"), 0, dwType, (PBYTE)&dwGetData, cbData);
			}
		}
		RegCloseKey(hKey);
	}

	hKey = NULL;

	if(ERROR_SUCCESS==RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),
		0,
		KEY_ALL_ACCESS | KEY_WOW64_64KEY,
		&hKey))
	{
		if (ERROR_SUCCESS==RegQueryValueEx(hKey, _T("1609"), 0, &dwType, (PBYTE)&dwGetData, &cbData))
		{
			if (dwGetData != 0)
			{
				dwGetData = 0;
				RegSetValueEx(hKey, _T("1609"), 0, dwType, (PBYTE)&dwGetData, cbData);
			}
		}
		RegCloseKey(hKey);
	}

	hKey = NULL;

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER,
		_T("Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),
		0,
		KEY_ALL_ACCESS | KEY_WOW64_64KEY,
		&hKey))
	{
		if (ERROR_SUCCESS ==RegQueryValueEx(hKey, _T("1609"), 0, &dwType, (PBYTE)&dwGetData, &cbData))
		{
			if (dwGetData != 0)
			{
				dwGetData = 0;
				RegSetValueEx(hKey, _T("1609"), 0, dwType, (PBYTE)&dwGetData, cbData);
			}
		}
		RegCloseKey(hKey);
	}

	hKey = NULL;

	if(ERROR_SUCCESS==RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		_T("Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3"),
		0,
		KEY_ALL_ACCESS | KEY_WOW64_64KEY,
		&hKey))
	{
		if (ERROR_SUCCESS==RegQueryValueEx(hKey, _T("1609"), 0, &dwType, (PBYTE)&dwGetData, &cbData))
		{
			if (dwGetData != 0)
			{
				dwGetData = 0;
				RegSetValueEx(hKey, _T("1609"), 0, dwType, (PBYTE)&dwGetData, cbData);
			}
		}
		RegCloseKey(hKey);
	}

	hKey = NULL;
}


CJsInjectorDlg::CJsInjectorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJsInjectorDlg::IDD, pParent)
	, m_strJsData(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CJsInjectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strJsData);
	DDX_Control(pDX, IDC_LIST1, m_wndListBox);
	DDX_Control(pDX, IDC_EDIT1, m_wndJsEdit);
}

BEGIN_MESSAGE_MAP(CJsInjectorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CJsInjectorDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT1, &CJsInjectorDlg::OnEnChangeEdit1)
	ON_MESSAGE(WM_USER+1122,OnDebugMsg)
	ON_BN_CLICKED(IDC_BUTTON1, &CJsInjectorDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CJsInjectorDlg 消息处理程序

BOOL CJsInjectorDlg::OnInitDialog()
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

	InternetSettingModify();

	g_pThis = this;
	InstallEventHook();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CJsInjectorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{

	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CJsInjectorDlg::OnPaint()
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
HCURSOR CJsInjectorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CJsInjectorDlg::OnBnClickedOk()
{
	UpdateData();

	if (m_strJsData.Left(4).CompareNoCase(L"http") == 0)
	{
		UpdateJsText(m_strJsData,1);
	}
	else
	{
		UpdateJsText(m_strJsData,0);
	}
	
}

void CJsInjectorDlg::OnEnChangeEdit1()
{
}

LRESULT CJsInjectorDlg::OnDebugMsg(WPARAM wParam,LPARAM lParam)
{
	WCHAR *pszData = (WCHAR *)wParam;
	m_wndListBox.AddString(pszData);
	delete pszData;

	m_wndListBox.SetCurSel(m_wndListBox.GetCount()-1);

	return 0;
}
void CJsInjectorDlg::OnBnClickedButton1()
{
	while (m_wndListBox.GetCount())
	{
		m_wndListBox.DeleteString(0);
	}
}
