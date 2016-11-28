
// DllInjecterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DllInjecter.h"
#include "DllInjecterDlg.h"
#include <TlHelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "Inject32Process.h"
#include "Inject64Process.h"


CDllInjecterDlg::CDllInjecterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDllInjecterDlg::IDD, pParent)
	, m_strProcInfo(_T(""))
	, m_strDllPath(_T(""))
	, m_strTestLoad(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDllInjecterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT3, m_strProcInfo);
	DDX_Text(pDX, IDC_EDIT2, m_strDllPath);
	DDX_Text(pDX, IDC_EDIT4, m_strTestLoad);
	DDX_Control(pDX, IDC_COMBO1, m_wndProcType);
}

BEGIN_MESSAGE_MAP(CDllInjecterDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CDllInjecterDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CDllInjecterDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDllInjecterDlg 消息处理程序

BOOL CDllInjecterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	
	m_wndProcType.InsertString(0,L"按进程名注入");
	m_wndProcType.InsertString(1,L"按进程ID注入");

	m_wndProcType.SetCurSel(0);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDllInjecterDlg::OnPaint()
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
HCURSOR CDllInjecterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


typedef enum SYSTEM_VERSION
{
	VERSION_ERROR = 0,
	VERSION_XP = 1,
	VERSION_WIN7 = 2,
	VERSION_WIN7_X64 = 3
}SYSTEM_VERSION;

BOOL IsWow64()
{
#ifdef _WIN64
	return TRUE;
#endif  
	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

	LPFN_ISWOW64PROCESS fnIsWow64Process;
	BOOL bIsWow64 = FALSE;    
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
		GetModuleHandle(TEXT("kernel32")),"IsWow64Process");      
	if (NULL != fnIsWow64Process)    
	{        
		if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
		{
			// handle error
		}
	}
	return bIsWow64;
}
/*
功能：获取系统版本信息，目前仅支持系统包括  win7_x64/x86  winxp_x86
返回值：系统版本
*/
//
SYSTEM_VERSION GetSystemVersion()
{
	SYSTEM_VERSION version = VERSION_ERROR;
	SYSTEM_INFO info;                //用SYSTEM_INFO结构判断64位AMD处理器
	GetSystemInfo(&info);            //调用GetSystemInfo函数填充结构
	OSVERSIONINFOEX os;
	os.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);
	if(GetVersionEx((OSVERSIONINFO *)&os))                  
	{
		//下面根据版本信息判断操作系统名称
		switch(os.dwMajorVersion)
		{
		case 5:
			{
				version = VERSION_XP;
			}
			break;
		case 6:
			{
#if WIN64 || _WIN64
				version = VERSION_WIN7_X64;
#else
				if(IsWow64())
				{
					version = VERSION_WIN7_X64;
				}
				else
				{
					version = VERSION_WIN7;
				}
#endif
			}
			break;
		}
	}

	return version;
}

BOOL InjectDLL(DWORD dwProcessID,LPCWSTR pszDllPath)
{

	BOOL b64Process = FALSE;

	if(IsWow64())
	{
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwProcessID);
		if (hProcess)
		{
			BOOL bWow64Process = FALSE;
			IsWow64Process(hProcess,&bWow64Process);
			if ( bWow64Process )
			{
				//32位
				b64Process = FALSE;
			}
			else
			{
				//64位
				b64Process = TRUE;
			}

			CloseHandle(hProcess);
		}
	}

	if (b64Process)
	{
		HANDLE hProcessHandle=OpenProcess(2035711,0,dwProcessID);
		InjectLibrary64(hProcessHandle,(WCHAR *)pszDllPath,wcslen(pszDllPath)*sizeof(WCHAR));
		CloseHandle(hProcessHandle);
	}
	else
	{
		InjectDll_RemoteThread(dwProcessID,pszDllPath,3000);
	}


	return TRUE;
}


void CDllInjecterDlg::OnBnClickedOk()
{
	UpdateData();

	if ( m_strDllPath.GetLength() == 0 )
	{
		AfxMessageBox(L"DLL路径不能为空");
		return;
	}

	if( FALSE == PathFileExistsW(m_strDllPath))
	{
		AfxMessageBox(m_strDllPath+L"不存在");
		return;
	}
	
	if( m_strProcInfo.GetLength() == 0)
	{
		AfxMessageBox( L"请输入要注入的进程信息" );
		return;
	}
	
	int nCurSel = m_wndProcType.GetCurSel();
	if ( nCurSel == 0 )
	{
		HANDLE   hProcessSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPALL,0);  
		PROCESSENTRY32 Info;
		Info.dwSize = sizeof(PROCESSENTRY32); 
		if(::Process32First(hProcessSnapshot,&Info))  
		{
			while(::Process32Next(hProcessSnapshot,&Info)!=FALSE)  
			{
				CString strExeFileName(Info.szExeFile);
				if( m_strProcInfo == L"*" || strExeFileName.CompareNoCase(m_strProcInfo) == 0 )
				{
					InjectDLL(Info.th32ProcessID,m_strDllPath);
				}

			}
			::CloseHandle(hProcessSnapshot);
			memset(&Info,0,sizeof(PROCESSENTRY32));
		}

		return ;
	}
	else if( 1 == nCurSel )
	{
		int nProcessID = 0;
		nProcessID = _ttoi(m_strProcInfo);
		if( nProcessID > 0 )
		{
			InjectDLL(nProcessID,m_strDllPath);
		}
		else
		{
			AfxMessageBox(L"请输入正确的进程ID");
		}
	}
}

void CDllInjecterDlg::OnBnClickedButton1()
{
	UpdateData();

	HMODULE hModule = LoadLibraryW(m_strTestLoad);
	if (hModule)
	{
		AfxMessageBox(L"加载成功");
	}
	else
	{
		CString strErrorText;
		strErrorText.Format(L"加载失败 ErrorCode %d",GetLastError());
		AfxMessageBox(strErrorText);
	}
}
