
// DllInjecterDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DllInjecter.h"
#include "DllInjecterDlg.h"
#include <TlHelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDllInjecterDlg �Ի���






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

//////////////////////////////////////////////////////////////////////////
//����		:�ͷ��ļ�
//����		:hMod - ��Դ����ģ����,souceId-��ԴID.,souceType-��Դ����,extraPath-�ͷ�·��
//����		:ReleaseFile
BOOL WINAPI ReleaseFile(HMODULE hMod,UINT souceId,LPCTSTR souceType,LPCTSTR extraPath)
{
	HRSRC hr;
	HANDLE hFile;
	hr=FindResource(hMod,MAKEINTRESOURCE(souceId),souceType);
	if(hr==NULL)
	{
		return FALSE;
	}  
	DWORD dwWritten,dwSize=SizeofResource(hMod,hr);
	HGLOBAL hg=LoadResource(hMod,hr);
	if(hg==NULL)
	{
		return FALSE;
	}
	LPCVOID lp=(LPCVOID)LockResource(hg);
	if(lp==NULL)
	{
		return FALSE;
	}

	hFile=CreateFile(extraPath,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
	if(hFile==INVALID_HANDLE_VALUE || hFile==NULL)
	{
		return FALSE;
	}
	WriteFile(hFile,(LPCVOID)lp,dwSize,&dwWritten,NULL);

	CloseHandle(hFile);
	return PathFileExists(extraPath);//
}

BOOL InjectDLL(DWORD dwProcessID,LPCWSTR pszDllPath)
{
	//ע��Ŀ����������64λϵͳ�е�32λ���̣���Ҫ���⴦��һ�£�����32λע��������ע��


	WCHAR szLocalPath[MAX_PATH]={0};
	GetModuleFileNameW(NULL,szLocalPath,MAX_PATH);
	WCHAR *pPathEnd = (WCHAR *)szLocalPath+wcslen(szLocalPath);
	while (pPathEnd != szLocalPath && *pPathEnd != L'\\') pPathEnd--;
	*(pPathEnd+1) = 0;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si,sizeof(si));
	ZeroMemory(&pi,sizeof(pi));
	si.cb = sizeof(si);


	CString strDllPath;
	strDllPath = pszDllPath;

	CString strCmdLine;
	strCmdLine.Format(L" -pid %d -dll %s",dwProcessID,strDllPath);
	
	CString strInjectorPath;
	BOOL b64BitProc = FALSE;

	strInjectorPath = szLocalPath;
	if(GetSystemVersion() == VERSION_WIN7_X64 )
	{
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwProcessID);
		if (hProcess)
		{
			BOOL bWow64Process = FALSE;
			IsWow64Process(hProcess,&bWow64Process);
			if ( bWow64Process )
			{
				strInjectorPath += L"Injector.exe";
			}
			else
			{
				b64BitProc = TRUE;
				strInjectorPath += L"Injector64.exe";
			}
		}
	}
	else
	{
		strInjectorPath += L"Injector.exe";
	}

	if ( FALSE == PathFileExistsW(strInjectorPath))
	{
		if (b64BitProc)
		{
			ReleaseFile(NULL,IDR_BIN2,L"BIN",strInjectorPath);
		}
		else
		{
			ReleaseFile(NULL,IDR_BIN1,L"BIN",strInjectorPath);
		}
	}

	BOOL bRes = CreateProcess( strInjectorPath ,strCmdLine.GetBuffer(),NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
	if (bRes && pi.hProcess)
	{
		WaitForSingleObject(pi.hProcess,5000);
		DWORD dwExitCode = 0 ;
		GetExitCodeProcess(pi.hProcess,&dwExitCode);
		bRes = dwExitCode == 0;
	}

	return bRes;
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
