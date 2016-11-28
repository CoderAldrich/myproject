
// DllInjecterDlg.cpp : ʵ���ļ�
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


// CDllInjecterDlg ��Ϣ�������

BOOL CDllInjecterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	
	m_wndProcType.InsertString(0,L"��������ע��");
	m_wndProcType.InsertString(1,L"������IDע��");

	m_wndProcType.SetCurSel(0);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CDllInjecterDlg::OnPaint()
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
���ܣ���ȡϵͳ�汾��Ϣ��Ŀǰ��֧��ϵͳ����  win7_x64/x86  winxp_x86
����ֵ��ϵͳ�汾
*/
//
SYSTEM_VERSION GetSystemVersion()
{
	SYSTEM_VERSION version = VERSION_ERROR;
	SYSTEM_INFO info;                //��SYSTEM_INFO�ṹ�ж�64λAMD������
	GetSystemInfo(&info);            //����GetSystemInfo�������ṹ
	OSVERSIONINFOEX os;
	os.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);
	if(GetVersionEx((OSVERSIONINFO *)&os))                  
	{
		//������ݰ汾��Ϣ�жϲ���ϵͳ����
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
				//32λ
				b64Process = FALSE;
			}
			else
			{
				//64λ
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
		AfxMessageBox(L"DLL·������Ϊ��");
		return;
	}

	if( FALSE == PathFileExistsW(m_strDllPath))
	{
		AfxMessageBox(m_strDllPath+L"������");
		return;
	}
	
	if( m_strProcInfo.GetLength() == 0)
	{
		AfxMessageBox( L"������Ҫע��Ľ�����Ϣ" );
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
			AfxMessageBox(L"��������ȷ�Ľ���ID");
		}
	}
}

void CDllInjecterDlg::OnBnClickedButton1()
{
	UpdateData();

	HMODULE hModule = LoadLibraryW(m_strTestLoad);
	if (hModule)
	{
		AfxMessageBox(L"���سɹ�");
	}
	else
	{
		CString strErrorText;
		strErrorText.Format(L"����ʧ�� ErrorCode %d",GetLastError());
		AfxMessageBox(strErrorText);
	}
}
