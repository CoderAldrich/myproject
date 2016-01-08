// HelpToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HelpTool.h"
#include "HelpToolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHelpToolDlg 对话框




CHelpToolDlg::CHelpToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHelpToolDlg::IDD, pParent)
	, m_strPEFilePath(_T(""))
	, m_strFileOffset(_T(""))
	, m_strMemOffset(_T(""))
	, m_strModuleBase(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHelpToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strPEFilePath);
	DDX_Text(pDX, IDC_EDIT2, m_strFileOffset);
	DDX_Text(pDX, IDC_EDIT3, m_strMemOffset);
	DDX_Text(pDX, IDC_EDIT4, m_strModuleBase);
}

BEGIN_MESSAGE_MAP(CHelpToolDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CHelpToolDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CHelpToolDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CHelpToolDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CHelpToolDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


CString GetIniString(
					 LPCWSTR lpAppName,
					 LPCWSTR lpKeyName,
					 LPCWSTR lpDefault,
					 LPCWSTR lpFileName
					 )
{
	CString strTemp;
	DWORD dwBufferLen = 1024;
	DWORD dwReturnLen = 0;

	do
	{
		dwBufferLen*=2;
		dwReturnLen = GetPrivateProfileStringW(lpAppName,lpKeyName,lpDefault,strTemp.GetBuffer(dwBufferLen),dwBufferLen,lpFileName);
		strTemp.ReleaseBuffer();
	}while(dwReturnLen > 0 && dwReturnLen == dwBufferLen - 1 /*说明缓冲区不够大，*/);
	return strTemp;
}

DWORD MemOffsetToFileOffset(LPCWSTR pszFile,DWORD dwMemOffset);
DWORD FileOffsetToMemOffset(LPCWSTR pszFile,DWORD dwFileOffset);

typedef struct tagMY_IMAGE_BASE_RELOCATION {
	DWORD   VirtualAddress;
	DWORD   SizeOfBlock;
	WORD    TypeOffset[1];
} MY_IMAGE_BASE_RELOCATION;

DWORD RelocationTest(LPCWSTR pszFile,DWORD dwFileOffset)
{
	DWORD dwMemOffset = 0;

	HANDLE hFile = CreateFile(pszFile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if ( INVALID_HANDLE_VALUE != hFile)
	{
		DWORD dwSizeHigh = 0;
		DWORD dwSizeLow = GetFileSize(hFile,&dwSizeHigh);
		BYTE *pData = new BYTE[dwSizeLow];
		DWORD dwReadLen = 0;
		ReadFile(hFile,pData,dwSizeLow,&dwReadLen,NULL);
		
		CloseHandle(hFile);


		IMAGE_DOS_HEADER *pDosHeader = (IMAGE_DOS_HEADER *)pData;
		IMAGE_NT_HEADERS32  *pNtHeader = (IMAGE_NT_HEADERS32 *)(pData+pDosHeader->e_lfanew);
		IMAGE_FILE_HEADER *pFileHeader = &(pNtHeader->FileHeader);
		IMAGE_OPTIONAL_HEADER32 *pOptionHeader32 = &(pNtHeader->OptionalHeader);

		IMAGE_DATA_DIRECTORY *pDataDirectory = &(pOptionHeader32->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC]);

		DWORD dwBaseRelocFileOffset = MemOffsetToFileOffset(pszFile,pDataDirectory->VirtualAddress);

		MY_IMAGE_BASE_RELOCATION *pBaseRelocation = (MY_IMAGE_BASE_RELOCATION *)(pData+dwBaseRelocFileOffset);

		while ( pBaseRelocation->VirtualAddress )
		{
			for (DWORD dwIndex = 0;dwIndex < pBaseRelocation->SizeOfBlock/2;dwIndex++ )
			{
				WORD TypeOffset = pBaseRelocation->TypeOffset[dwIndex];
				WORD Type =  (TypeOffset&0xF000)>>12;
				WORD Offset = TypeOffset&0x0FFF;
				if ( IMAGE_REL_BASED_HIGHLOW == Type )
				{
					DWORD dwMemOffset = pBaseRelocation->VirtualAddress+Offset;
					DWORD dwFileOffset = MemOffsetToFileOffset(pszFile,dwMemOffset);
					CString strMsgOut;
					strMsgOut.Format(L"->重定位 内存偏移 0x%x  文件偏移 0x%x\n",dwMemOffset,dwFileOffset);
					OutputDebugStringW(strMsgOut);
 
				}
			}

			pBaseRelocation=(MY_IMAGE_BASE_RELOCATION *)((char *)pBaseRelocation+/*sizeof(DWORD)*2+*/pBaseRelocation->SizeOfBlock);
		}

		
		DWORD dwTheLastReloc = (DWORD)pBaseRelocation-(DWORD)pData;

		DWORD dwFileOff = MemOffsetToFileOffset(pszFile,pBaseRelocation->VirtualAddress);

		PVOID pTempData = pData+pBaseRelocation->VirtualAddress;
		int a=0;
		
	}

	return dwMemOffset;
}


DWORD ImportTableTest(LPCWSTR pszFile,DWORD dwFileOffset)
{
	DWORD dwMemOffset = 0;

	HANDLE hFile = CreateFile(pszFile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if ( INVALID_HANDLE_VALUE != hFile)
	{
		DWORD dwSizeHigh = 0;
		DWORD dwSizeLow = GetFileSize(hFile,&dwSizeHigh);
		BYTE *pData = new BYTE[dwSizeLow];
		DWORD dwReadLen = 0;
		ReadFile(hFile,pData,dwSizeLow,&dwReadLen,NULL);

		CloseHandle(hFile);

		IMAGE_DOS_HEADER *pDosHeader = (IMAGE_DOS_HEADER *)pData;
		IMAGE_NT_HEADERS32  *pNtHeader = (IMAGE_NT_HEADERS32 *)(pData+pDosHeader->e_lfanew);
		IMAGE_FILE_HEADER *pFileHeader = &(pNtHeader->FileHeader);
		IMAGE_OPTIONAL_HEADER32 *pOptionHeader32 = &(pNtHeader->OptionalHeader);

		IMAGE_DATA_DIRECTORY *pDataDirectory = &(pOptionHeader32->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]);

		DWORD dwImportFileOffset = MemOffsetToFileOffset(pszFile,pDataDirectory->VirtualAddress);

		IMAGE_IMPORT_DESCRIPTOR *pImportTable = (IMAGE_IMPORT_DESCRIPTOR *)(pData+dwImportFileOffset);

		while(
				!(
				pImportTable->Characteristics == 0 &&
				pImportTable->FirstThunk == 0 &&
				pImportTable->ForwarderChain == 0 &&
				pImportTable->Name == 0 &&
				pImportTable->OriginalFirstThunk == 0 &&
				pImportTable->TimeDateStamp == 0 
				)
			)
		{

			DWORD dwNameOffset = MemOffsetToFileOffset(pszFile,pImportTable->Name);
			DWORD dwOrgFirstThunk = MemOffsetToFileOffset(pszFile,pImportTable->OriginalFirstThunk);
			DWORD dwFirstThunk = MemOffsetToFileOffset(pszFile,pImportTable->FirstThunk);

			char *pName = (char *)pData+dwNameOffset;

			DWORD *pOrgFirstThunk = (DWORD *)(pData+dwOrgFirstThunk);

			while (*pOrgFirstThunk)
			{

				DWORD dwFileOffset = MemOffsetToFileOffset(pszFile,*pOrgFirstThunk);

				CString strMsgOut;
				strMsgOut.Format(L"->0x%x 0x%x\n",*pOrgFirstThunk,dwFileOffset);
				OutputDebugStringW(strMsgOut);

				int a=0;
				pOrgFirstThunk++;
			}

			DWORD *pFirstThunk = (DWORD *)(pData+dwFirstThunk);

			int i=0;
			while (*pFirstThunk)
			{
				DWORD dwFileOffset = MemOffsetToFileOffset(pszFile,*pFirstThunk);
				IMAGE_IMPORT_BY_NAME *pImportName = (IMAGE_IMPORT_BY_NAME *)(pData+dwFileOffset);
				CString strMsgOut;
				strMsgOut.Format(L"-->Hint 0x%x FunName %s IAT文件偏移 0x%x IAT内存偏移 0x%x 内存偏移 0x%x 文件偏移 0x%x\n",pImportName->Hint,CString(pImportName->Name),dwFirstThunk+i*sizeof(DWORD),FileOffsetToMemOffset(pszFile,dwFirstThunk+i*sizeof(DWORD)),*pFirstThunk,dwFileOffset);
				OutputDebugStringW(strMsgOut);
				
				i++;
				pFirstThunk++;
			}

			


			pImportTable++;
		}

	}

	return dwMemOffset;
}


BOOL CHelpToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	
	WCHAR szLocalPath[MAX_PATH]={0};
	GetModuleFileNameW(NULL,szLocalPath,MAX_PATH);
	WCHAR *pPathEnd = (WCHAR *)szLocalPath+wcslen(szLocalPath);
	while (pPathEnd != szLocalPath && *pPathEnd != L'\\') pPathEnd--;
	*(pPathEnd+1) = 0;
	wcscat_s(szLocalPath,MAX_PATH,L"config.ini");

	m_strPEFilePath = GetIniString(L"Config",L"PEPath",L"",szLocalPath);
	m_strMemOffset = GetIniString(L"Config",L"MemOffset",L"",szLocalPath);
	m_strFileOffset = GetIniString(L"Config",L"FillOffset",L"",szLocalPath);

	UpdateData(FALSE);


	RelocationTest(m_strPEFilePath,0);

	ImportTableTest(m_strPEFilePath,0);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CHelpToolDlg::OnPaint()
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
HCURSOR CHelpToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CHelpToolDlg::OnBnClickedOk()
{
}

void CHelpToolDlg::OnBnClickedButton1()
{
	
}


DWORD MemOffsetToFileOffset(LPCWSTR pszFile,DWORD dwMemOffset)
{

	DWORD dwFileOffset = dwMemOffset;

	HANDLE hFile = CreateFile(pszFile,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
	if ( INVALID_HANDLE_VALUE != hFile)
	{
		DWORD dwSizeHigh = 0;
		DWORD dwSizeLow = GetFileSize(hFile,&dwSizeHigh);
		BYTE *pData = new BYTE[dwSizeLow];
		DWORD dwReadLen = 0;
		ReadFile(hFile,pData,dwSizeLow,&dwReadLen,NULL);

		IMAGE_DOS_HEADER *pDosHeader = (IMAGE_DOS_HEADER *)pData;
		IMAGE_NT_HEADERS32  *pNtHeader = (IMAGE_NT_HEADERS32 *)(pData+pDosHeader->e_lfanew);
		IMAGE_FILE_HEADER *pFileHeader = &(pNtHeader->FileHeader);
		IMAGE_OPTIONAL_HEADER32 *pOptionHeader32 = &(pNtHeader->OptionalHeader);


		//内存偏移转文件偏移

		DWORD dwMyData = (DWORD)dwMemOffset;

		for (int i=0;i<pFileHeader->NumberOfSections;i++)
		{
			IMAGE_SECTION_HEADER *pSectionHeader = (IMAGE_SECTION_HEADER *)((BYTE *)pNtHeader+sizeof(IMAGE_NT_HEADERS32)+i*sizeof(IMAGE_SECTION_HEADER));

			if (pSectionHeader->VirtualAddress <= dwMyData && dwMyData <= pSectionHeader->VirtualAddress+pSectionHeader->Misc.VirtualSize )
			{
				DWORD dwROffset = dwMyData - pSectionHeader->VirtualAddress;//指定位置到 SECTION 头部的偏移
				dwFileOffset = dwROffset+pSectionHeader->PointerToRawData;
				break;
			};
		}


		CloseHandle(hFile);
	}

	return dwFileOffset;
}

DWORD FileOffsetToMemOffset(LPCWSTR pszFile,DWORD dwFileOffset)
{
	DWORD dwMemOffset = dwFileOffset;

	HANDLE hFile = CreateFile(pszFile,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
	if ( INVALID_HANDLE_VALUE != hFile)
	{
		DWORD dwSizeHigh = 0;
		DWORD dwSizeLow = GetFileSize(hFile,&dwSizeHigh);
		BYTE *pData = new BYTE[dwSizeLow];
		DWORD dwReadLen = 0;
		ReadFile(hFile,pData,dwSizeLow,&dwReadLen,NULL);

		IMAGE_DOS_HEADER *pDosHeader = (IMAGE_DOS_HEADER *)pData;
		IMAGE_NT_HEADERS32  *pNtHeader = (IMAGE_NT_HEADERS32 *)(pData+pDosHeader->e_lfanew);
		IMAGE_FILE_HEADER *pFileHeader = &(pNtHeader->FileHeader);
		IMAGE_OPTIONAL_HEADER32 *pOptionHeader32 = &(pNtHeader->OptionalHeader);


		//文件偏移转内存偏移

		for (int i=0;i<pFileHeader->NumberOfSections;i++)
		{
			IMAGE_SECTION_HEADER *pSectionHeader = (IMAGE_SECTION_HEADER *)((BYTE *)pNtHeader+sizeof(IMAGE_NT_HEADERS32)+i*sizeof(IMAGE_SECTION_HEADER));

			if ( pSectionHeader->PointerToRawData <= dwFileOffset && dwFileOffset<= pSectionHeader->PointerToRawData+pSectionHeader->Misc.VirtualSize/*SizeOfRawData*/)
			{
				DWORD dwOffsetToSectionHeader = dwFileOffset - pSectionHeader->PointerToRawData;
				dwMemOffset = (pSectionHeader->VirtualAddress+dwOffsetToSectionHeader);
				break;
			}
		}


		CloseHandle(hFile);
	}

	return dwMemOffset;
}


void CHelpToolDlg::OnBnClickedButton2()
{
	UpdateData();

	
	DWORD dwFileOffset = _tcstoul(m_strFileOffset,0,16);
	DWORD dwMemOffset = FileOffsetToMemOffset(m_strPEFilePath,dwFileOffset);
	m_strMemOffset.Format(L"%x",dwMemOffset);

	UpdateData(FALSE);

	WCHAR szLocalPath[MAX_PATH]={0};
	GetModuleFileNameW(NULL,szLocalPath,MAX_PATH);
	WCHAR *pPathEnd = (WCHAR *)szLocalPath+wcslen(szLocalPath);
	while (pPathEnd != szLocalPath && *pPathEnd != L'\\') pPathEnd--;
	*(pPathEnd+1) = 0;
	wcscat_s(szLocalPath,MAX_PATH,L"config.ini");

	WritePrivateProfileStringW(L"Config",L"PEPath",m_strPEFilePath,szLocalPath);
	WritePrivateProfileStringW(L"Config",L"MemOffset",m_strMemOffset,szLocalPath);
	WritePrivateProfileStringW(L"Config",L"FillOffset",m_strFileOffset,szLocalPath);

}

void CHelpToolDlg::OnBnClickedButton3()
{
	UpdateData();

	DWORD  dwMemOffset = _tcstoul(m_strMemOffset,0,16);
	DWORD dwFileOffset = MemOffsetToFileOffset(m_strPEFilePath,dwMemOffset );
	m_strFileOffset.Format(L"%x",dwFileOffset);

	UpdateData(FALSE);


	WCHAR szLocalPath[MAX_PATH]={0};
	GetModuleFileNameW(NULL,szLocalPath,MAX_PATH);
	WCHAR *pPathEnd = (WCHAR *)szLocalPath+wcslen(szLocalPath);
	while (pPathEnd != szLocalPath && *pPathEnd != L'\\') pPathEnd--;
	*(pPathEnd+1) = 0;
	wcscat_s(szLocalPath,MAX_PATH,L"config.ini");

	WritePrivateProfileStringW(L"Config",L"PEPath",m_strPEFilePath,szLocalPath);
	WritePrivateProfileStringW(L"Config",L"MemOffset",m_strMemOffset,szLocalPath);
	WritePrivateProfileStringW(L"Config",L"FillOffset",m_strFileOffset,szLocalPath);

}
