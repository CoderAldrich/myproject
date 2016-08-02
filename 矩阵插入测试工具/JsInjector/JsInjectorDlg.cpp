
// JsInjectorDlg.cpp : ʵ���ļ�
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


// CJsInjectorDlg ��Ϣ�������

BOOL CJsInjectorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	InternetSettingModify();

	g_pThis = this;
	InstallEventHook();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CJsInjectorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
